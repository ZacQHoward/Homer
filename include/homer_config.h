#pragma once
//Config file for Homer

#include <Arduino.h>

namespace HomerConfig {

    //----------TRANSLATIONAL DRIFT SETTINGS----------
    constexpr float ACCEL_MOUNT_RADIUS_CM = 5.3f;              //Radius of accelerometer from center of robot                      
    constexpr float DEFAULT_ACCEL_ZERO_G_OFFSET = 2.75f;       //Offset value (in G) used to get 0G accelerometer reading when robot is at rest, For small-radius bots openmelt recommends trying H3LIS331 at +/-200g range (MED_RANGE) for improved accuracy (accel_handler.h)
    constexpr float BASE_SPIN_MAX_OFFSET_PERCENTAGE = 0.4f;    //Offset percentage value (0.0f - 1.0f) to limit base spin speed below full ESC range so translation modulation still has headroom at high throttle

    //May need this later
    constexpr uint16_t MIN_TRANSLATION_RPM = 400;                            //full power spin in below this number (increasing can reduce spin-up time)

    //----------PIN MAPPINGS----------
    constexpr uint8_t MOTOR_PIN1 = D0;  //Pin for Motor 1 driver (right motor if weapon facing away from you, yellow wire solo pin in mr30)
    constexpr uint8_t MOTOR_PIN2 = D1;  //Pin for Motor 2 driver (left motor if weapon facing away from you, red wire solo pin in mr30)

    constexpr uint8_t ACCEL_SCL_PIN = D2;   //Pin for SCL on accelerometer (clock)
    constexpr uint8_t ACCEL_SDA_PIN = D3;   //Pin for SDA on accelerometer (data)

    constexpr uint8_t HEADING_LED_RED_PIN = D4;     //Pin for red component of heading LED
    constexpr uint8_t HEADING_LED_GREEN_PIN = D5;   //Pin for green component of heading LED
    constexpr uint8_t HEADING_LED_BLUE_PIN = D6;    //Pin for blue component of heading LED

    //RC receiver setup:
    //CH1 = left/right, CH2 = forward/back, CH3 = throttle, CH4 = trim/orientation bump
    constexpr uint8_t LEFTRIGHT_RC_CHANNEL_PIN = D7;        //Pin for CH1 on RC receiver
    constexpr uint8_t FORBACK_RC_CHANNEL_PIN = D8;          //Pin for CH2 on RC receiver
    constexpr uint8_t THROTTLE_RC_CHANNEL_PIN = D9;         //Pin for CH3 on RC receiver
    constexpr uint8_t ORIENTATION_RC_CHANNEL_PIN = D10;     //Pin for CH4 / orientation trim on RC receiver

}  // namespace HomerConfig