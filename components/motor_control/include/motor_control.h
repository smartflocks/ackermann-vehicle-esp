// Description: Motor control module for the ESP32. This module uses the MCPWM peripheral to control the motor.
#pragma once

#include <esp_err.h>
#include <esp_attr.h>
#include <esp_check.h>

#include <esp_log.h>
#include <driver/mcpwm_prelude.h>
#include "driver/mcpwm_timer.h"

typedef struct motor_control_t *motor_control_handle_t;

typedef enum {
    MOTOR_CONTROL_DIRECTION_FORWARD,
    MOTOR_CONTROL_DIRECTION_BACKWARD,
    MOTOR_CONTROL_DIRECTION_BRAKE,
    MOTOR_CONTROL_DIRECTION_COAST,
} motor_control_direction;


esp_err_t motor_get_count(motor_control_handle_t motor_control, int *count);
esp_err_t motor_enable(motor_control_handle_t motor_control);
esp_err_t motor_disable(motor_control_handle_t motor_control);
esp_err_t motor_set_direction(motor_control_handle_t motor_control, motor_control_direction direction);
esp_err_t motor_set_speed(motor_control_handle_t motor_control, uint32_t speed);


typedef struct {
    int group_id;
    uint32_t pwm_gpio_a;
    uint32_t pwm_gpio_b;
    uint32_t pwm_frequency;
    uint32_t pwm_resolution;
    int encoder_gpio_a;
    int encoder_gpio_b;
    int high_limit;
    int low_limit;
    uint32_t max_glitch_ns;

} motor_control_config_t;

esp_err_t motor_control_init(motor_control_config_t *config, motor_control_handle_t *motor_control);