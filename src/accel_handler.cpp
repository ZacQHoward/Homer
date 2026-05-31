// This file interfaces with the accelerometer to provide current G-force level given the specified orientation axis (x, y, z)

#include "accel_handler.h"
#include <Arduino.h>
#include <Wire.h>
#include "homer_config.h"

static LIS331 xl;

// Setup for I2C connection and initializing the accelerometer with the specified settings
void init_accel() {

	// Init I2C data/clock pins and set frequency
	Wire.begin(HomerConfig::ACCEL_SDA_PIN, HomerConfig::ACCEL_SCL_PIN, AccelConfig::I2C_FREQUENCY_HZ);

	xl.setI2CAddr(AccelConfig::ACCEL_I2C_ADDRESS);
	xl.begin(LIS331::USE_I2C);
	xl.setFullScale(AccelConfig::ACCEL_RANGE); // Sets accelerometer to specified scale (100, 200, 400g)

}

// Helper function to read raw accelerometer values into x, y, z variables for debugging or other purposes
void get_accel_axes(int16_t &x, int16_t &y, int16_t &z) {
	xl.readAxes(x, y, z);
}

// Gets the reading from the selected axis
static int16_t get_selected_axis(int16_t x, int16_t y, int16_t z) {

	switch (AccelConfig::ACCEL_FORCE_AXIS) {
    	case LIS331::X_AXIS:  return x;
    	case LIS331::Y_AXIS:  return y;
    	case LIS331::Z_AXIS:  return z;
    	default: return z;
  	}

}

// Reads accel and converts to G's, ACCEL_MAX_SCALE needs to match ACCEL_RANGE value (handled for you in the homer_config.h header file)
float get_accel_force_g() {

	int16_t x, y, z;

  	get_accel_axes(x, y, z);
  	return xl.convertToG(AccelConfig::ACCEL_MAX_SCALE, get_selected_axis(x, y, z));

}