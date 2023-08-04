#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/pulse_cnt.h"
#include "bdc_motor.h"
#include "pid_ctrl.h"
#include "motor_control.h"

static const char *TAG = "example";

// Enable this config,  we will print debug formated string, which in return can be captured and parsed by Serial-Studio
#define SERIAL_STUDIO_DEBUG           CONFIG_SERIAL_STUDIO_DEBUG

#define BDC_MCPWM_TIMER_RESOLUTION_HZ 10000000 // 10MHz, 1 tick = 0.1us
#define BDC_MCPWM_FREQ_HZ             250000    // 250KHz PWM
#define BDC_MCPWM_DUTY_TICK_MAX       (BDC_MCPWM_TIMER_RESOLUTION_HZ / BDC_MCPWM_FREQ_HZ) // maximum value we can set for the duty cycle, in ticks
#define BDC_MCPWM_A_GPIO_A              18
#define BDC_MCPWM_A_GPIO_B              19
#define BDC_MCPWM_B_GPIO_A              5
#define BDC_MCPWM_B_GPIO_B              17

#define BDC_ENCODER_A_GPIO_A            33
#define BDC_ENCODER_A_GPIO_B            32
#define BDC_ENCODER_B_GPIO_A            25
#define BDC_ENCODER_B_GPIO_B            26
#define BDC_ENCODER_PCNT_HIGH_LIMIT   1000
#define BDC_ENCODER_PCNT_LOW_LIMIT    -1000

#define BDC_PID_LOOP_PERIOD_MS        10   // calculate the motor speed every 10ms
#define BDC_PID_EXPECT_SPEED          15  // expected motor speed, in the pulses counted by the rotary encoder

typedef struct {
    motor_control_handle_t motor_control_a;
    motor_control_handle_t motor_control_b;
    pid_ctrl_block_handle_t pid_ctrl_a;
    pid_ctrl_block_handle_t pid_ctrl_b;
} motor_control_context_t;

int pid_loop(motor_control_handle_t motor, pid_ctrl_block_handle_t pid_ctrl, int expected_speed, int last_pulse_count)
{
    // get the result from rotary encoder
    int cur_pulse_count = 0;
    motor_get_count(motor, &cur_pulse_count);
    int real_pulses = cur_pulse_count - last_pulse_count;


    // calculate the speed error
    float error = expected_speed - real_pulses;
    float new_speed = 0;

    // set the new speed
    pid_compute(pid_ctrl, error, &new_speed);
    //ESP_LOGI(TAG, "Error %f.2", error);
    motor_set_speed(motor, (uint32_t)new_speed);
    return cur_pulse_count;
}

static void pid_loop_cb(void *args)
{
    static int last_pulse_count_a = 0;
    static int last_pulse_count_b = 0;
    motor_control_context_t *ctx = (motor_control_context_t *)args;
    pid_ctrl_block_handle_t pid_ctrl_a = ctx->pid_ctrl_a;
    pid_ctrl_block_handle_t pid_ctrl_b = ctx->pid_ctrl_b;
    motor_control_handle_t motor_a = ctx->motor_control_a;
    motor_control_handle_t motor_b = ctx->motor_control_b;
    last_pulse_count_a = pid_loop(motor_a, pid_ctrl_a, BDC_PID_EXPECT_SPEED, last_pulse_count_a);
    last_pulse_count_b = pid_loop(motor_b, pid_ctrl_b, BDC_PID_EXPECT_SPEED, last_pulse_count_b);
    
}

void app_main(void)
{
    static motor_control_context_t motor_ctrl_ctx = {
        .motor_control_a = NULL,
        .motor_control_b = NULL,
        .pid_ctrl_a = NULL,
        .pid_ctrl_b = NULL,
    };

    ESP_LOGI(TAG, "Initialize motor control");

    motor_control_config_t motor_control_config_a = {
        .group_id = 0,
        .pwm_gpio_a = BDC_MCPWM_A_GPIO_A,
        .pwm_gpio_b = BDC_MCPWM_A_GPIO_B,
        .pwm_frequency = BDC_MCPWM_FREQ_HZ,
        .pwm_resolution = BDC_MCPWM_TIMER_RESOLUTION_HZ,
        .encoder_gpio_a = BDC_ENCODER_A_GPIO_A,
        .encoder_gpio_b = BDC_ENCODER_A_GPIO_B,
        .high_limit = BDC_ENCODER_PCNT_HIGH_LIMIT,
        .low_limit = BDC_ENCODER_PCNT_LOW_LIMIT,
        .max_glitch_ns = 100,
    };

    ESP_ERROR_CHECK(motor_control_init(&motor_control_config_a, &motor_ctrl_ctx.motor_control_a));

    motor_control_config_t motor_control_config_b = {
        .group_id = 1,
        .pwm_gpio_a = BDC_MCPWM_B_GPIO_A,
        .pwm_gpio_b = BDC_MCPWM_B_GPIO_B,
        .pwm_frequency = BDC_MCPWM_FREQ_HZ,
        .pwm_resolution = BDC_MCPWM_TIMER_RESOLUTION_HZ,
        .encoder_gpio_a = BDC_ENCODER_B_GPIO_A,
        .encoder_gpio_b = BDC_ENCODER_B_GPIO_B,
        .high_limit = BDC_ENCODER_PCNT_HIGH_LIMIT,
        .low_limit = BDC_ENCODER_PCNT_LOW_LIMIT,
        .max_glitch_ns = 100,
    };

    ESP_ERROR_CHECK(motor_control_init(&motor_control_config_b, &motor_ctrl_ctx.motor_control_b));

    ESP_LOGI(TAG, "Create PID control block");
    pid_ctrl_parameter_t pid_runtime_param = {
        .kp = 0.6,
        .ki = 0.4,
        .kd = 0.2,
        .cal_type = PID_CAL_TYPE_INCREMENTAL,
        .max_output   = BDC_MCPWM_DUTY_TICK_MAX - 1,
        .min_output   = 0,
        .max_integral = 1000,
        .min_integral = -1000,
    };
    
    pid_ctrl_config_t pid_config = {
        .init_param = pid_runtime_param,
    };

    ESP_ERROR_CHECK(pid_new_control_block(&pid_config, &motor_ctrl_ctx.pid_ctrl_a));

    ESP_ERROR_CHECK(pid_new_control_block(&pid_config, &motor_ctrl_ctx.pid_ctrl_b));
    

    ESP_LOGI(TAG, "Create a timer to do PID calculation periodically");
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = pid_loop_cb,
        .arg = &motor_ctrl_ctx,
        .name = "pid_loop"
    };
    esp_timer_handle_t pid_loop_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &pid_loop_timer));

    ESP_LOGI(TAG, "Enable motor");
    ESP_ERROR_CHECK(motor_enable(motor_ctrl_ctx.motor_control_a));
    ESP_ERROR_CHECK(motor_enable(motor_ctrl_ctx.motor_control_b));
    ESP_LOGI(TAG, "Forward motor");
    ESP_ERROR_CHECK(motor_set_direction(motor_ctrl_ctx.motor_control_a, MOTOR_CONTROL_DIRECTION_FORWARD));
    ESP_ERROR_CHECK(motor_set_direction(motor_ctrl_ctx.motor_control_b, MOTOR_CONTROL_DIRECTION_FORWARD));

    ESP_LOGI(TAG, "Start motor speed loop");
    ESP_ERROR_CHECK(esp_timer_start_periodic(pid_loop_timer, BDC_PID_LOOP_PERIOD_MS * 1000));

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
        // the following logging format is according to the requirement of serial-studio frame format
        // also see the dashboard config file `serial-studio-dashboard.json` for more information
        int pulse_count_a = 0;
        int pulse_count_b = 0;
        ESP_ERROR_CHECK(motor_get_count(motor_ctrl_ctx.motor_control_a, &pulse_count_a));
        ESP_ERROR_CHECK(motor_get_count(motor_ctrl_ctx.motor_control_b, &pulse_count_b));

        ESP_LOGI(TAG, "Pulse count motor A: %d", pulse_count_a);
        ESP_LOGI(TAG, "Pulse count motor B: %d", pulse_count_b);

    }
}