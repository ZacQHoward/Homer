#pragma once

#include <stdint.h>

void init_motors();

void motor_1_write_us(uint16_t pulse_width_us);
void motor_2_write_us(uint16_t pulse_width_us);

void motor_1_off();
void motor_2_off();
void motors_off();