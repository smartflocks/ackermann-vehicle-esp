
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/mcpwm_prelude.h"

#include "pwm_generator.h"

static const char *TAG = "example";

// Please consult the datasheet of your servo before changing the following parameters
#define SERVO_MIN_PULSEWIDTH_US 500  // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US 2500  // Maximum pulse width in microsecond
#define SERVO_MIN_DEGREE        -90   // Minimum angle
#define SERVO_MAX_DEGREE        90    // Maximum angle

#define SERVO_PULSE_GPIO             1        // GPIO connects to the PWM signal line
#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD        20000    // 20000 ticks, 20ms

static inline uint32_t example_angle_to_compare(int angle)
{
    return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

void app_main(void)
{
    ESP_LOGI(TAG, "Create timer and operator");
    pwm_generator_handle_t generator = NULL;
    pwm_generator_handle_t generator2 = NULL;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ,
        .period_ticks = SERVO_TIMEBASE_PERIOD,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };

    ESP_ERROR_CHECK(pwm_generator_init(&generator, SERVO_PULSE_GPIO, 1500, &timer_config));
    ESP_ERROR_CHECK(pwm_generator_init(&generator2, 2, 1500, &timer_config));

    int angle = 0;
    int step = 1;
    ESP_LOGI(TAG, "Starting loop");
    while (1) {
        
        uint32_t duty = example_angle_to_compare(angle);
        uint32_t duty2 = example_angle_to_compare(-angle);
        ESP_ERROR_CHECK(pwm_generator_set_dutycycle(generator, duty));
        ESP_ERROR_CHECK(pwm_generator_set_dutycycle(generator2, duty2));
        ESP_LOGI(TAG, "Dutycycle: %d ", (int)duty);
        ESP_LOGI(TAG, "Dutycycle2: %d ", (int)duty2);
        //Add delay, since it takes time for servo to rotate, usually 200ms/60degree rotation under 5V power supply
        vTaskDelay(pdMS_TO_TICKS(20));
        if ((angle + step) > 90 || (angle + step) < -90) {
            step *= -1;
        }
        angle += step;
    }
}
