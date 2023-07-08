#include "pwm_capture.h"

const static char *TAG = "example";

typedef struct callback_data_t
{
    int gpio;
    TaskHandle_t * task;
    uint32_t cap_val_begin_of_sample;
    uint32_t cap_val_end_of_sample;
} callback_data_t;


bool hc_sr04_echo_callback(mcpwm_cap_channel_handle_t cap_chan, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    
    //TaskHandle_t task_to_notify = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup = pdFALSE;
    callback_conf_t * conf = (callback_conf_t *)user_data;
    //calculate the interval in the ISR,
    //so that the interval will be always correct even when capture_queue is not handled in time and overflow.
    if (edata->cap_edge == MCPWM_CAP_EDGE_POS) {
        // store the timestamp when pos edge is detected
        conf->cap_val_begin_of_sample = edata->cap_value;
        conf->cap_val_end_of_sample = conf->cap_val_begin_of_sample;
    } else {
        conf->cap_val_end_of_sample = edata->cap_value;
        uint32_t tof_ticks = conf->cap_val_end_of_sample - conf->cap_val_begin_of_sample;

        // notify the task to calculate the distance
        xTaskNotifyFromISR(conf->task, tof_ticks, eSetValueWithOverwrite, &high_task_wakeup);
    }

    return high_task_wakeup == pdTRUE;
}

esp_err_t
register_pwm_callback(
    callback_conf_t * callback_conf,
    const mcpwm_capture_timer_config_t * conf)
{
    mcpwm_cap_timer_handle_t cap_timer = NULL;

    ESP_LOGI(TAG, "Initialize capture timer");

    ESP_RETURN_ON_ERROR(mcpwm_new_capture_timer(conf, &cap_timer), TAG, "Failed to initialize the timer");
    
    mcpwm_cap_channel_handle_t cap_chan = NULL;
    mcpwm_capture_channel_config_t cap_ch_conf = {
        .gpio_num = callback_conf->gpio,
        .prescale = 1,
        // capture on both edge
        .flags.neg_edge = true,
        .flags.pos_edge = true,
        // pull up internally
        .flags.pull_up = true,
    };

    ESP_LOGI(TAG, "Initialize capture channel");

    ESP_RETURN_ON_ERROR(mcpwm_new_capture_channel(cap_timer, &cap_ch_conf, &cap_chan), TAG, "Failed to initialize the capture channel");

    ESP_LOGI(TAG, "Register capture callback");

    mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = hc_sr04_echo_callback,
    };

    ESP_RETURN_ON_ERROR(mcpwm_capture_channel_register_event_callbacks(cap_chan, &cbs, (void *)callback_conf), TAG, "Failed to register the capture callback");

    ESP_LOGI(TAG, "Enable capture channel");
    ESP_RETURN_ON_ERROR(mcpwm_capture_channel_enable(cap_chan), TAG, "Failed to enable the capture channel");

    ESP_LOGI(TAG, "Enable and start capture timer");
    ESP_RETURN_ON_ERROR(mcpwm_capture_timer_enable(cap_timer), TAG, "Failed to enable the capture timer");
    ESP_RETURN_ON_ERROR(mcpwm_capture_timer_start(cap_timer), TAG, "Failed to start the capture timer");

    return ESP_OK;
}