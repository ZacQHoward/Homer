//This file interfaces with the accelerometer to provide current G-force level given the specified orientation axis (x, y, z)

#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_LIS331.h"

#include "homer_config.h"
#include "accel_handler.h"

static LIS331 xl;

// Setup for I2C connection and initializing the accelerometer with the specified settings
void init_accel() {

	Wire.setPins(HomerConfig::ACCEL_SDA_PIN, HomerConfig::ACCEL_SCL_PIN);  // Uses the pins defined in homer_config.h instead of the board default I2C pins
	Wire.begin();
  
	Wire.setClock(400000);  //increase I2C speed to reduce read times a bit
                          	//value of 400000 allows accel read in ~1ms and
                          	//is verfied to work with Sparkfun level converter
                          	//(some level converters have issues at higher speeds)
  
	xl.setI2CAddr(AccelConfig::ACCEL_I2C_ADDRESS);
	xl.begin(LIS331::USE_I2C);
	xl.setFullScale(AccelConfig::ACCEL_RANGE);  //sets accelerometer to specified scale (100, 200, 400g)

}

// Helper function to read raw accelerometer values into x, y, z variables for debugging or other purposes
void get_accel_axes(int16_t &x, int16_t &y, int16_t &z) {
	xl.readAxes(x, y, z);
}

// Gets the reading from the selected axis
static int16_t get_selected_axis(int16_t x, int16_t y, int16_t z) {

	switch (AccelConfig::ACCEL_FORCE_AXIS) {
    	case 0:  return x;
    	case 1:  return y;
    	case 2:  return z;
    	default: return z;
  	}

}

//Reads accel and converts to G's, ACCEL_MAX_SCALE needs to match ACCEL_RANGE value (handled for you in the accel_handler.h header file)
float get_accel_force_g() {

	int16_t x = 0;
  	int16_t y = 0;
  	int16_t z = 0;

  	get_accel_axes(x, y, z);
  	return xl.convertToG(AccelConfig::ACCEL_MAX_SCALE, get_selected_axis(x, y, z));

}