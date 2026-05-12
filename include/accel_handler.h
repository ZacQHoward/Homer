#pragma once

#include <Arduino.h>
#include "SparkFun_LIS331.h"

namespace AccelConfig {

    //Set high enough to allow for G forces at top RPM
    //LOW_RANGE - +/-100g for the H3LIS331DH
    //MED_RANGE - +/-200g for the H3LIS331DH
    //HIGH_RANGE - +/-400g for the H3LIS331DH
    constexpr auto ACCEL_RANGE = LIS331::HIGH_RANGE;

    constexpr int getAccelScale(uint8_t range) {
        return (range == LIS331::LOW_RANGE)  ? 100 :
               (range == LIS331::MED_RANGE)  ? 200 :
               (range == LIS331::HIGH_RANGE) ? 400 :
                                               400;
    }
    constexpr int ACCEL_MAX_SCALE = getAccelScale(ACCEL_RANGE);

    //Adafuit breakout default is 0x18, Sparkfun default is 0x19
    constexpr uint8_t ACCEL_I2C_ADDRESS = 0x19;

    //Select which axis points toward the robot center (0 = X, 1 = Y, 2 = Z)
    constexpr uint8_t ACCEL_FORCE_AXIS = 2;

}  // namespace AccelConfig

void init_accel();  // Setup I2C and initialize accelerometer
void get_accel_axes(int16_t &x, int16_t &y, int16_t &z);    //Read raw accelerometer axis values
float get_accel_force_g();  //Read accelerometer and convert to G's based on selected axis and scale