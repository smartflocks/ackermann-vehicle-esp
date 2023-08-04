#include <stdio.h>
#include <math.h>
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

#include "pwm_capture.h"

static const char *TAG = "two_wheel_control";

// Enable this config,  we will print debug formated string, which in return can be captured and parsed by Serial-Studio
#define SERIAL_STUDIO_DEBUG           CONFIG_SERIAL_STUDIO_DEBUG

#define CAPTURE_GPIO_A 13
#define CAPTURE_GPIO_B 14

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


#define WHEEL_RADIUS 0.03 // 30.0mm
#define BASE_WIDTH 0.09 // 90.0mm

typedef struct {
    motor_control_handle_t motor_control_a;
    motor_control_handle_t motor_control_b;
    pid_ctrl_block_handle_t pid_ctrl_a;
    pid_ctrl_block_handle_t pid_ctrl_b;
    QueueHandle_t throtle_queue;
} motor_control_context_t;

typedef struct motor_control_input_t {
    int32_t throtle;
    int32_t stearing;
} motor_control_input_t;

static int32_t scale_value(uint32_t output)
{
    float old_min = 500;
    float old_max = 2500;
    float new_min = -35;
    float new_max = 35;

    // Scale the value to the new range
    float new_value = (output - old_min) * (new_max - new_min) / (old_max - old_min) + new_min;

    return new_value;
}


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
    //ESP_LOGI(TAG, "Error %f.2", new_speed);
    motor_control_direction new_direction = new_speed > 0 ? MOTOR_CONTROL_DIRECTION_FORWARD : MOTOR_CONTROL_DIRECTION_BACKWARD;
        
    motor_set_direction(motor, new_direction);
    
    motor_set_speed(motor, (uint32_t)fabs(new_speed));
    return cur_pulse_count;
}

static void pid_loop_cb(void *args)
{
    static int last_pulse_count_a = 0;
    static int last_pulse_count_b = 0;
    static int32_t throtle = 0;
    static int32_t stearing = 0;
    motor_control_context_t *ctx = (motor_control_context_t *)args;
    pid_ctrl_block_handle_t pid_ctrl_a = ctx->pid_ctrl_a;
    pid_ctrl_block_handle_t pid_ctrl_b = ctx->pid_ctrl_b;
    motor_control_handle_t motor_a = ctx->motor_control_a;
    motor_control_handle_t motor_b = ctx->motor_control_b;
    int32_t new_throtle = 0;
    if(xQueueReceiveFromISR(ctx->throtle_queue, &new_throtle, NULL) == pdPASS ){
        throtle = new_throtle;
    }
    //ESP_LOGI(TAG, "Throtle: %d", (int)throtle);
    last_pulse_count_a = pid_loop(motor_a, pid_ctrl_a, throtle, last_pulse_count_a);
    last_pulse_count_b = pid_loop(motor_b, pid_ctrl_b, throtle, last_pulse_count_b);
    
}

void app_main(void)
{
    static motor_control_context_t motor_ctrl_ctx = {
        .motor_control_a = NULL,
        .motor_control_b = NULL,
        .pid_ctrl_a = NULL,
        .pid_ctrl_b = NULL,
        .throtle_queue = NULL,
    };

    motor_ctrl_ctx.throtle_queue = xQueueCreate(1, sizeof(int32_t));

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
        .min_output   = -BDC_MCPWM_DUTY_TICK_MAX + 1,
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

    motor_control_direction current_direction = MOTOR_CONTROL_DIRECTION_FORWARD;

    ESP_LOGI(TAG, "Start motor speed loop");
    ESP_ERROR_CHECK(esp_timer_start_periodic(pid_loop_timer, BDC_PID_LOOP_PERIOD_MS * 1000));

    int last_pulse_count_a = 0;
    int last_pulse_count_b = 0;

    while (1) {
        
        vTaskDelay(pdMS_TO_TICKS(20));
        pwm_output_t tof_ticks_a;
        ESP_ERROR_CHECK(pwm_capture_get_duty_width(capture_a, &tof_ticks_a));
        pwm_output_t tof_ticks_b;
        ESP_ERROR_CHECK(pwm_capture_get_duty_width(capture_b, &tof_ticks_b));

        int32_t throtle = 0;
        int32_t steering = 0;

        motor_control_direction new_direction = tof_ticks_a.width_us > 1500 ? MOTOR_CONTROL_DIRECTION_FORWARD : MOTOR_CONTROL_DIRECTION_BACKWARD;
        if(current_direction != new_direction)
        {
            ESP_LOGI(TAG, "Change direction");
            int32_t zero_speed = 0;
            xQueueOverwrite(motor_ctrl_ctx.throtle_queue, &zero_speed);
            pid_reset_ctrl_block(motor_ctrl_ctx.pid_ctrl_a);
            pid_reset_ctrl_block(motor_ctrl_ctx.pid_ctrl_b);
            
            current_direction = new_direction;
        }


        throtle = scale_value(tof_ticks_a.width_us);
        steering = scale_value(tof_ticks_b.width_us);

        //ESP_LOGI(TAG, "Speed: %d", (int)speed);
        //ESP_LOGI(TAG, "Current PWM A: %d ", (int)tof_ticks_b.width_us);
        xQueueOverwrite(motor_ctrl_ctx.throtle_queue, &throtle);      

    }
}