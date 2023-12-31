#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_private/esp_clk.h"
#include "driver/mcpwm_cap.h"
#include "driver/gpio.h"

#include "pwm_capture.h"

const static char *TAG = "capture_demo";


#define CAPTURE_GPIO_A 13
#define CAPTURE_GPIO_B 14

void app_main(void)
{


    pwm_capture_handle_t capture_a =NULL;

    pwm_capture_conf_t config_a = {
        .gpio = CAPTURE_GPIO_A,
        .group_id = 0,
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
    };

    pwm_capture_handle_t capture_b =NULL;

    pwm_capture_conf_t config_b = {
        .gpio = CAPTURE_GPIO_B,
        .group_id = 1,
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
    };

    pwm_capture_init(&capture_a, &config_a);
    pwm_capture_init(&capture_b, &config_b);
    while (1) {
        pwm_output_t tof_ticks_a;
        pwm_capture_get_duty_width(capture_a, &tof_ticks_a);

        pwm_output_t tof_ticks_b;
        pwm_capture_get_duty_width(capture_b, &tof_ticks_b);
        ESP_LOGI(TAG, "Current PWM A: %d ", (int)tof_ticks_a.width_us);
        ESP_LOGI(TAG, "Current PWM B: %d ", (int)tof_ticks_b.width_us);
        vTaskDelay(pdMS_TO_TICKS(20));
    }

}
