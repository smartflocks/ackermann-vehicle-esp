#include "pwm_hook.h"

const static char *TAG = "pwm_hook";

#define MAX_TIMESTAMP 60000



void pwm_logger_task(void * pvParameters)
{

    pwm_logger_conf_t * config = (pwm_logger_conf_t *) pvParameters;
    pwm_capture_handle_t capture = NULL;

    pwm_generator_handle_t generator = NULL;

    ESP_ERROR_CHECK(pwm_generator_init(&generator, config->gpio, config->zero, &config->timer_config));

    pwm_capture_init(&capture, &config->capture_config);

    uint32_t last_timestamp = 0;
    uint32_t current_timestamp = 0;

    while (1) {

        pwm_output_t tof_ticks;

        pwm_capture_get_duty_width(capture, &tof_ticks);

        if(tof_ticks.timestamp < last_timestamp)
        {

            current_timestamp += (tof_ticks.timestamp + (MAX_TIMESTAMP - last_timestamp));
            
        }
        else
        {

            current_timestamp += tof_ticks.timestamp - last_timestamp;

        }

        last_timestamp = tof_ticks.timestamp;
        tof_ticks.timestamp = current_timestamp;

        uint32_t value = config->filter(&tof_ticks);

        ESP_ERROR_CHECK(pwm_generator_set_dutycycle(generator, value));      

        vTaskDelay(pdMS_TO_TICKS(config->delay));
    }

}
