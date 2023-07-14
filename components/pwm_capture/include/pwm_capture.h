#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <esp_err.h>
#include <esp_attr.h>
#include <esp_check.h>

#include <esp_private/esp_clk.h>
#include <driver/mcpwm_cap.h>
#include <driver/gpio.h>

typedef struct pwm_capture_t *pwm_capture_handle_t;

typedef struct pwm_capture_t pwm_capture_t;

typedef struct {
    int gpio;
    int group_id;
    mcpwm_capture_clock_source_t clk_src;
} pwm_capture_conf_t;

typedef struct pwm_output_t {
    uint32_t width;
    uint32_t width_us;
    uint32_t timestamp;
} pwm_output_t; 

struct pwm_capture_t {
    esp_err_t (*get_duty_width)(pwm_capture_t *pwm_capture, pwm_output_t * width);
};

esp_err_t
pwm_capture_init(
    pwm_capture_handle_t * pwm_capture,
    const pwm_capture_conf_t * conf);

esp_err_t
pwm_capture_get_duty_width(
    pwm_capture_handle_t pwm_capture,
    pwm_output_t * width);
