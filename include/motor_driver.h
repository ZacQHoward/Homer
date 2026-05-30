#pragma once

#include <stdint.h>

void init_motors();
void motors_stop();
void motors_write_normalized(float motor_1_output, float motor_2_output);