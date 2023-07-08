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

#include "lib/pwm_capture.h"

const static char *TAG = "example";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your board spec ////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LISTEN_GPIO  13


void blink_task(void *pvParameter);

void app_main(void)
{

    TaskHandle_t xHandle = NULL;



    xTaskCreate(blink_task,
                "stering_task",
                4096,
                (void *)14,
                9,
                NULL);

    xTaskCreate(blink_task,
                "throtle_task",
                4096,
                (void *)13,
                10,
                NULL);

    while (1) {
        
        vTaskDelay(pdMS_TO_TICKS(200));
    }

}

void blink_task(void *pvParameter)
{
    int gpio = (int *)pvParameter;
    mcpwm_capture_timer_config_t cap_conf = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        .group_id = gpio == 13 ? 0 : 1,
    };

    callback_conf_t callback_conf = {
        .gpio = gpio,
        .task = xTaskGetCurrentTaskHandle(),
        .cap_val_begin_of_sample = 0,
        .cap_val_end_of_sample = 0,
    };

    
    register_pwm_callback(&callback_conf, &cap_conf);
    uint32_t current_pwm = 0;
    uint32_t tof_ticks;
    
    while (1) {
        // wait for echo done signal
        if (xTaskNotifyWait(0x00, ULONG_MAX, &tof_ticks, pdMS_TO_TICKS(1000)) == pdTRUE) {
            //ESP_LOGI(TAG, "This never happens %d ", (int)tof_ticks);
            current_pwm = tof_ticks * (1000000.0 / esp_clk_apb_freq());
        }
        ESP_LOGI(TAG, "Current PWM: %d ", (int)current_pwm);
        vTaskDelay(pdMS_TO_TICKS(20));

    }
}