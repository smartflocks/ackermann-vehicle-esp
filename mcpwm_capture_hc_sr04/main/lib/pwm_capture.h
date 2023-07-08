#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_err.h>
#include <esp_attr.h>
#include <esp_check.h>

#include <esp_private/esp_clk.h>
#include <driver/mcpwm_cap.h>
#include <driver/gpio.h>

typedef struct callback_conf_t callback_conf_t;

typedef struct callback_conf_t
{
    int gpio;
    TaskHandle_t * task;
    uint32_t cap_val_begin_of_sample;
    uint32_t cap_val_end_of_sample;
} callback_conf_t;

typedef struct callback_data_t *callback_data_handle_t;

typedef struct callback_data_t callback_data_t;


esp_err_t
register_pwm_callback(
    callback_conf_t * callback_conf,
    const mcpwm_capture_timer_config_t * conf);