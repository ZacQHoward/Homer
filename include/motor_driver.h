#pragma once

#include <stdint.h>

void init_motors(); // Initializes motor PWM channels and attaches pins, should be called in setup()
void motors_stop(); // Stops the motors by writing the neutral pulse width to both motor channels
void motors_write_normalized(float motor_1_input, float motor_2_input); // Writes normalized motor inputs (-1.0 to 1.0) to the motors, where -1.0 is full clockwise, 0.0 is stop, and 1.0 is full counter-clockwise if looking at the motor from shaft side (double check this)

void write_motor_us(uint8_t pwm_channel, uint16_t pulse_width_us); // Writes a specific pulse width in microseconds to a motor's PWM channel, with clamping to min/max values (not intended for public use but can be used for testing or debugging if needed)