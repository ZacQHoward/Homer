#pragma once

#include <stdint.h>

void init_motors();

void motor_1_write_us(uint16_t pulse_width_us);
void motor_2_write_us(uint16_t pulse_width_us);

void motor_1_neutral();
void motor_2_neutral();
void motors_stop();