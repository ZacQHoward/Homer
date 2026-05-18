#pragma once

#include <stdint.h>

void init_accel();  // Setup I2C and initialize accelerometer
void get_accel_axes(int16_t &x, int16_t &y, int16_t &z);    //Read raw accelerometer axis values
float get_accel_force_g();  //Read accelerometer and convert to G's based on selected axis and scale