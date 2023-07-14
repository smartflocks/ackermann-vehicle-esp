idf_component_register(/
    SRCS pwm_hook.c
    INCLUDE_DIRS include
    REQUIRES pwm_capture pwm_generator
)