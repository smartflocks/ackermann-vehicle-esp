#include "esp_err.h"

#include "motor_control.h"

typedef struct motor_control_t motor_control_t;

struct motor_control_t
{
    esp_err_t (*get_count)(motor_control_t *self, int *count);
    esp_err_t (*enable)(motor_control_t *self);
    esp_err_t (*disable)(motor_control_t *self);
    esp_err_t (*set_speed)(motor_control_t *self, uint32_t speed);
    esp_err_t (*set_direction)(motor_control_t *self, motor_control_direction direction);
};
