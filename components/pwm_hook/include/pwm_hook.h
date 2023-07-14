#include "pwm_capture.h"
#include "pwm_generator.h"

typedef struct {
    int gpio;
    uint32_t zero;
    uint32_t delay;
    pwm_capture_conf_t capture_config;
    mcpwm_timer_config_t timer_config;
    uint32_t (* filter)(pwm_output_t * output);
} pwm_logger_conf_t;

void pwm_logger_task(void * pvParameters);