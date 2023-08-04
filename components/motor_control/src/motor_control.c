
#include "motor_control.h"
#include "motor_control_interface.h"
#include "driver/pulse_cnt.h"
#include "bdc_motor.h"
#include "pid_ctrl.h"

static const char *TAG = "motor_control";

typedef struct {
    motor_control_t base;
    bdc_motor_handle_t motor;
    pcnt_unit_handle_t pcnt_encoder;
    int report_pulses;
} motor_control_impl_t;

static esp_err_t motor_control_get_count(motor_control_handle_t motor_control, int *count)
{
    motor_control_impl_t *impl = __containerof(motor_control, motor_control_impl_t, base);
    return pcnt_unit_get_count(impl->pcnt_encoder, count);
}

static esp_err_t motor_control_enable(motor_control_handle_t motor_control)
{
    motor_control_impl_t *impl = __containerof(motor_control, motor_control_impl_t, base);
    return bdc_motor_enable(impl->motor);
}

static esp_err_t motor_control_disable(motor_control_handle_t motor_control)
{
    motor_control_impl_t *impl = __containerof(motor_control, motor_control_impl_t, base);
    return bdc_motor_disable(impl->motor);
}

static esp_err_t motor_control_set_direction(motor_control_handle_t motor_control, motor_control_direction direction)
{
    motor_control_impl_t *impl = __containerof(motor_control, motor_control_impl_t, base);
    switch (direction) {
        case MOTOR_CONTROL_DIRECTION_FORWARD:
            return bdc_motor_forward(impl->motor);
        case MOTOR_CONTROL_DIRECTION_BACKWARD:
            return bdc_motor_reverse(impl->motor);
        case MOTOR_CONTROL_DIRECTION_BRAKE:
            return bdc_motor_brake(impl->motor);
        case MOTOR_CONTROL_DIRECTION_COAST:
            return bdc_motor_coast(impl->motor);
        default:
            return ESP_ERR_INVALID_ARG;
    }
}

static esp_err_t motor_control_set_speed(motor_control_handle_t motor_control, uint32_t speed)
{
    motor_control_impl_t *impl = __containerof(motor_control, motor_control_impl_t, base);
    return bdc_motor_set_speed(impl->motor, speed);
}

esp_err_t motor_control_init(motor_control_config_t *config, motor_control_handle_t *motor_control)
{
    motor_control_impl_t *impl = calloc(1, sizeof(motor_control_impl_t));
    
    if (!impl) {
        ESP_LOGE(TAG, "Failed to allocate memory for motor control");
        return ESP_ERR_NO_MEM;
    }

    bdc_motor_mcpwm_config_t mcpwm_config = {
        .group_id = config->group_id,
        .resolution_hz = config->pwm_resolution,
    };

    bdc_motor_config_t motor_config = {
        .pwm_freq_hz = config->pwm_frequency,
        .pwma_gpio_num = config->pwm_gpio_a,
        .pwmb_gpio_num = config->pwm_gpio_b,
    };

    ESP_ERROR_CHECK(bdc_motor_new_mcpwm_device(&motor_config, &mcpwm_config, &impl->motor));

    pcnt_unit_config_t pcnt_config = {
        .high_limit = config->high_limit,
        .low_limit = config->low_limit,
        .flags.accum_count = true,
    };

    ESP_ERROR_CHECK(pcnt_new_unit(&pcnt_config, &impl->pcnt_encoder));

    pcnt_glitch_filter_config_t glitch_filter_config = {
        .max_glitch_ns = config->max_glitch_ns,
    };

    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(impl->pcnt_encoder, &glitch_filter_config));

    pcnt_chan_config_t pcnt_chan_a_config = {
        .edge_gpio_num = config->encoder_gpio_a,
        .level_gpio_num = config->encoder_gpio_b,
    };

    pcnt_channel_handle_t pcnt_chan_a;
    ESP_ERROR_CHECK(pcnt_new_channel(impl->pcnt_encoder, &pcnt_chan_a_config, &pcnt_chan_a));

    pcnt_chan_config_t pcnt_chan_b_config = {
        .edge_gpio_num = config->encoder_gpio_b,
        .level_gpio_num = config->encoder_gpio_a,
    };

    pcnt_channel_handle_t pcnt_chan_b;
    ESP_ERROR_CHECK(pcnt_new_channel(impl->pcnt_encoder, &pcnt_chan_b_config, &pcnt_chan_b));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));

    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(impl->pcnt_encoder, config->high_limit));
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(impl->pcnt_encoder, config->low_limit));

    ESP_ERROR_CHECK(pcnt_unit_enable(impl->pcnt_encoder));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(impl->pcnt_encoder));
    ESP_ERROR_CHECK(pcnt_unit_start(impl->pcnt_encoder));

    impl->base.get_count = motor_control_get_count;
    impl->base.enable = motor_control_enable;
    impl->base.disable = motor_control_disable;
    impl->base.set_direction = motor_control_set_direction;
    impl->base.set_speed = motor_control_set_speed;

    *motor_control = &impl->base;

    return ESP_OK;

}

esp_err_t motor_get_count(motor_control_handle_t motor_control, int *count)
{
    return motor_control->get_count(motor_control, count);
}

esp_err_t motor_enable(motor_control_handle_t motor_control)
{
    return motor_control->enable(motor_control);
}

esp_err_t motor_disable(motor_control_handle_t motor_control)
{
    return motor_control->disable(motor_control);
}

esp_err_t motor_set_direction(motor_control_handle_t motor_control, motor_control_direction direction)
{
    return motor_control->set_direction(motor_control, direction);
}

esp_err_t motor_set_speed(motor_control_handle_t motor_control, uint32_t speed)
{
    return motor_control->set_speed(motor_control, speed);
}