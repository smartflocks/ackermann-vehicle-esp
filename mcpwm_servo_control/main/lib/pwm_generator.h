# pragma once

#include <esp_err.h>
#include <esp_attr.h>
#include <esp_check.h>

#include <esp_log.h>
#include <driver/mcpwm_prelude.h>
#include "driver/mcpwm_timer.h"

typedef struct pwm_generator_t *pwm_generator_handle_t;

typedef struct pwm_generator_t pwm_generator_t;

struct pwm_generator_t {
    esp_err_t (*set_duty)(pwm_generator_t *pwm_generator, int32_t dc);
};


esp_err_t
pwm_generator_init(
    pwm_generator_handle_t * pwm_generator,
    const int pulse_gpio,
    const int start_pulse,
    const mcpwm_timer_config_t * options);

esp_err_t
pwm_generator_set_dutycycle(
    pwm_generator_handle_t pwm_generator,
    int32_t dc);

