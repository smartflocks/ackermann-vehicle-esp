idf_component_register(/
    SRCS "src/motor_control.c"
    INCLUDE_DIRS "include" "interface"
    REQUIRES "driver" "bdc_motor" "pid_ctrl")