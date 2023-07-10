
#include "pwm_generator.h"

static const char *TAG = "pwm_generator";

esp_err_t pwm_generator_set_dutycycle(pwm_generator_handle_t pwm_generator, int32_t dc)
{
    ESP_RETURN_ON_FALSE(pwm_generator, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return pwm_generator->set_duty(pwm_generator, dc);
}


typedef struct
{
    pwm_generator_t base;
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t oper;
    mcpwm_cmpr_handle_t comparator;
    mcpwm_gen_handle_t generator;

}pwm_generator_impl_t;


static esp_err_t
pwm_set_dutycycle(
    pwm_generator_t * pwm_generator,
    int32_t dc)
{
    pwm_generator_impl_t *pwm_generator_impl = __containerof(pwm_generator, pwm_generator_impl_t, base);
    return mcpwm_comparator_set_compare_value(pwm_generator_impl->comparator, dc);

}

esp_err_t
pwm_generator_init(
    pwm_generator_handle_t * pwm_generator,
    const int pulse_gpio,
    const int start_pulse,
    const mcpwm_timer_config_t * timer_config)
{

    pwm_generator_impl_t *pwm_generator_impl = NULL;

    pwm_generator_impl = calloc(1, sizeof(pwm_generator_impl_t));
    
    ESP_RETURN_ON_ERROR(mcpwm_new_timer(timer_config, &pwm_generator_impl->timer), TAG, "Failed to initialize the timer");

    
    
    mcpwm_operator_config_t operator_config = {
        .group_id = timer_config->group_id // operator must be in the same group to the timer
    };


    ESP_RETURN_ON_ERROR(mcpwm_new_operator(&operator_config, &pwm_generator_impl->oper), TAG, "Failed to initialize the operator");

    ESP_LOGI(TAG, "Connect timer and operator");
    ESP_RETURN_ON_ERROR(mcpwm_operator_connect_timer(pwm_generator_impl->oper, pwm_generator_impl->timer), TAG, "Unable to connect timer with oprator");


    ESP_LOGI(TAG, "Create comparator and generator from the operator");

    
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };

    ESP_RETURN_ON_ERROR(mcpwm_new_comparator(pwm_generator_impl->oper, &comparator_config, &pwm_generator_impl->comparator), TAG, "Unable to create comparator");

    
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = pulse_gpio,
    };

    ESP_RETURN_ON_ERROR(mcpwm_new_generator(pwm_generator_impl->oper, &generator_config, &pwm_generator_impl->generator),TAG, "Failed to initialize the generator");

    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(pwm_generator_impl->comparator, start_pulse), TAG, "Failed to compere values");

    ESP_LOGI(TAG, "Set generator action on timer and compare event");
    // go high on counter empty
    ESP_RETURN_ON_ERROR(mcpwm_generator_set_action_on_timer_event(pwm_generator_impl->generator,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)), TAG, "Failed to go high");
    // go low on compare threshold
    ESP_RETURN_ON_ERROR(mcpwm_generator_set_action_on_compare_event(pwm_generator_impl->generator,
                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, pwm_generator_impl->comparator, MCPWM_GEN_ACTION_LOW)), TAG, "Failed to go low");


    ESP_LOGI(TAG, "Enable and start timer");

    ESP_RETURN_ON_ERROR(mcpwm_timer_enable(pwm_generator_impl->timer), TAG, "Unable to enable the timer");
    ESP_RETURN_ON_ERROR(mcpwm_timer_start_stop(pwm_generator_impl->timer, MCPWM_TIMER_START_NO_STOP), TAG, "Unable to start the timer");

    ESP_LOGI(TAG, "Timer enable and running");

    pwm_generator_impl->base.set_duty = pwm_set_dutycycle;

    ESP_LOGI(TAG, "Functions register in base");

    *pwm_generator = &pwm_generator_impl->base;

    ESP_LOGI(TAG, "Initialization ended");

    return ESP_OK;
}

