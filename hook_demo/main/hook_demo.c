#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_private/esp_clk.h"
#include "driver/mcpwm_cap.h"
#include "driver/gpio.h"

#include "pwm_hook.h"

const static char *TAG = "hook_demo";

#define SERVO_PULSE_GPIO_A             1        // GPIO connects to the PWM signal line
#define SERVO_PULSE_GPIO_B             2        // GPIO connects to the PWM signal line

#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD        20000    // 20000 ticks, 20ms

#define CAPTURE_GPIO_A 13
#define CAPTURE_GPIO_B 14

static uint32_t log_value(pwm_output_t * output)
{
    ESP_LOGI(TAG, "Ellapsed time: %d ", (int)output->timestamp);
    return output->width_us;
}

static uint32_t scale_value(pwm_output_t * output)
{
    uint32_t new_val = (output->width_us - 1000) * (2500 - 500) / (2000 - 1000) + 500;
    return new_val;
}

void app_main(void)
{
    pwm_logger_conf_t config_a = {
        .gpio = SERVO_PULSE_GPIO_A,
        .zero = 1500,
        .delay = 20,
        .capture_config = {
            .gpio = CAPTURE_GPIO_A,
            .group_id = 0,
            .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        },

        .timer_config = {
            .group_id = 0,
            .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
            .resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ,
            .period_ticks = SERVO_TIMEBASE_PERIOD,
            .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        },

        .filter = scale_value,
    };

    pwm_logger_conf_t config_b = {
        .gpio = SERVO_PULSE_GPIO_B,
        .zero = 1500,
        .delay = 20,
        .capture_config = {
            .gpio = CAPTURE_GPIO_B,
            .group_id = 1,
            .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        },

        .timer_config = {
            .group_id = 1,
            .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
            .resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ,
            .period_ticks = SERVO_TIMEBASE_PERIOD,
            .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        },

        .filter = log_value,
    };

    xTaskCreate(pwm_logger_task,
                "throtle_task",
                4096,
                (void *)&config_a,
                1,
                NULL);

    xTaskCreate(pwm_logger_task,
                "steering_task",
                4096,
                (void *)&config_b,
                1,
                NULL);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

}
