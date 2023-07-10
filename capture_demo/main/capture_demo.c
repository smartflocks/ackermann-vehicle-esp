/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_private/esp_clk.h"
#include "driver/mcpwm_cap.h"
#include "driver/gpio.h"

#include "pwm_capture.h"

const static char *TAG = "example";


void app_main(void)
{


    pwm_capture_handle_t capture =NULL;

    pwm_capture_conf_t config = {
        .gpio = 13,
        .group_id = 0,
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
    };

    pwm_capture_handle_t capture2 =NULL;

    pwm_capture_conf_t config2 = {
        .gpio = 14,
        .group_id = 1,
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
    };

    pwm_capture_init(&capture, &config);
    pwm_capture_init(&capture2, &config2);
    while (1) {
        uint32_t tof_ticks;
        pwm_capture_get_dutycycle(capture, &tof_ticks);

        uint32_t tof_ticks2;
        pwm_capture_get_dutycycle(capture2, &tof_ticks2);
        ESP_LOGI(TAG, "Current PWM Servo: %d ", (int)tof_ticks2);
        ESP_LOGI(TAG, "Current PWM Motor: %d ", (int)tof_ticks);
        vTaskDelay(pdMS_TO_TICKS(20));
    }

}
