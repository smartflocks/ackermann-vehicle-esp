#include "pwm_capture.h"

const static char *TAG = "pwm_capture";


typedef struct
{
    pwm_capture_t base;
    mcpwm_cap_timer_handle_t cap_timer;
    mcpwm_cap_channel_handle_t cap_chan;
    uint32_t cap_val_begin_of_sample;
    uint32_t cap_val_end_of_sample;
    uint32_t resolution;
    uint32_t frequency;
    QueueHandle_t queue;

} pwm_capture_impl_t;

esp_err_t
pwm_capture_get_duty_width(
    pwm_capture_handle_t pwm_capture,
    pwm_output_t * width)
{
    return pwm_capture->get_duty_width(pwm_capture, width);
}

static esp_err_t
pwm_get_duty_width(
    pwm_capture_t * pwm_capture,
    pwm_output_t * width)
{
    pwm_capture_impl_t *pwm_capture_impl = __containerof(pwm_capture, pwm_capture_impl_t, base);
    if( xQueueReceive(pwm_capture_impl->queue,
                            width,
                            ( TickType_t ) 10 ) == pdPASS )
    {
        return ESP_OK;
    }

    return ESP_FAIL;

}

bool pwm_capture_callback(mcpwm_cap_channel_handle_t cap_chan, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    
    //TaskHandle_t task_to_notify = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup = pdFALSE;
    pwm_capture_impl_t * impl = (pwm_capture_impl_t *)user_data;
    //calculate the interval in the ISR,
    //so that the interval will be always correct even when capture_queue is not handled in time and overflow.
    if (edata->cap_edge == MCPWM_CAP_EDGE_POS) {
        // store the timestamp when pos edge is detected
        impl->cap_val_begin_of_sample = edata->cap_value;
        impl->cap_val_end_of_sample = impl->cap_val_begin_of_sample;
    } else {
        impl->cap_val_end_of_sample = edata->cap_value;
        uint32_t tof_ticks = impl->cap_val_end_of_sample - impl->cap_val_begin_of_sample;

        // push value to the queue
        pwm_output_t pwm_output = {
            .width = tof_ticks, // Send width
            .width_us = tof_ticks * (1000000.0 /impl->frequency), // Send width in US
            .timestamp = (edata->cap_value * 1000.0)/impl->resolution, // Send timestamp bounded to 60s
        };
        xQueueOverwriteFromISR( impl->queue, &pwm_output, &high_task_wakeup);

    }

    return high_task_wakeup == pdTRUE;
}

esp_err_t
pwm_capture_init(
    pwm_capture_handle_t * pwm_capture,
    const pwm_capture_conf_t * conf)
{
    pwm_capture_impl_t *pwm_capture_impl = NULL;

    pwm_capture_impl = calloc(1, sizeof(pwm_capture_impl_t));

    pwm_capture_impl->cap_val_begin_of_sample = 0;
    pwm_capture_impl->cap_val_end_of_sample = 0;
    pwm_capture_impl->frequency = esp_clk_apb_freq();

    pwm_capture_impl->queue = xQueueCreate( 1, sizeof(struct pwm_output_t) );

    mcpwm_capture_timer_config_t cap_conf = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        .group_id = conf->group_id,
    };

    ESP_LOGI(TAG, "Initialize capture timer");

    ESP_RETURN_ON_ERROR(mcpwm_new_capture_timer(&cap_conf, &pwm_capture_impl->cap_timer), TAG, "Failed to initialize the timer");

    ESP_RETURN_ON_ERROR(mcpwm_capture_timer_get_resolution(pwm_capture_impl->cap_timer, &pwm_capture_impl->resolution), TAG, "Failed to get timer resolution");

    mcpwm_capture_channel_config_t cap_ch_conf = {
        .gpio_num = conf->gpio,
        .prescale = 1,
        // capture on both edge
        .flags.neg_edge = true,
        .flags.pos_edge = true,
        // pull up internally
        .flags.pull_up = true,
    };

    ESP_LOGI(TAG, "Initialize capture channel");

    ESP_RETURN_ON_ERROR(mcpwm_new_capture_channel(pwm_capture_impl->cap_timer, &cap_ch_conf, &pwm_capture_impl->cap_chan), TAG, "Failed to initialize the capture channel");

    ESP_LOGI(TAG, "Register capture callback");

    mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = pwm_capture_callback,
    };

    ESP_RETURN_ON_ERROR(mcpwm_capture_channel_register_event_callbacks(pwm_capture_impl->cap_chan, &cbs, (void *)pwm_capture_impl), TAG, "Failed to register the capture callback");

    ESP_LOGI(TAG, "Enable capture channel");
    ESP_RETURN_ON_ERROR(mcpwm_capture_channel_enable(pwm_capture_impl->cap_chan), TAG, "Failed to enable the capture channel");

    ESP_LOGI(TAG, "Enable and start capture timer");
    ESP_RETURN_ON_ERROR(mcpwm_capture_timer_enable(pwm_capture_impl->cap_timer), TAG, "Failed to enable the capture timer");
    ESP_RETURN_ON_ERROR(mcpwm_capture_timer_start(pwm_capture_impl->cap_timer), TAG, "Failed to start the capture timer");

    ESP_LOGI(TAG, "Timer enable and running");

    pwm_capture_impl->base.get_duty_width = pwm_get_duty_width;

    ESP_LOGI(TAG, "Functions register in base");

    *pwm_capture = &pwm_capture_impl->base;

    ESP_LOGI(TAG, "Initialization ended");

    return ESP_OK;
}