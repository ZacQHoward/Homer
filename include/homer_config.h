#pragma once
//Config file for Homer, contains all project namespaces in alphabetical order

#include <Arduino.h>
#include "SparkFun_LIS331.h"

namespace AccelConfig {

    //Set high enough to allow for G forces at top RPM
    //LOW_RANGE - +/-100g for the H3LIS331DH
    //MED_RANGE - +/-200g for the H3LIS331DH
    //HIGH_RANGE - +/-400g for the H3LIS331DH
    constexpr LIS331::fs_range ACCEL_RANGE = LIS331::HIGH_RANGE;

    // Helper function to get scale from range
    constexpr int getAccelScale(LIS331::fs_range range) {
        return (range == LIS331::LOW_RANGE)  ? 100 :
               (range == LIS331::MED_RANGE)  ? 200 :
               (range == LIS331::HIGH_RANGE) ? 400 :
                                               400;
    }
    constexpr int ACCEL_MAX_SCALE = getAccelScale(ACCEL_RANGE);

    // Increase I2C speed to reduce read times a bit value of 400000 allows accel read in ~1ms
    // verified to work with Sparkfun level converter (some level converters have issues at higher speeds)
    constexpr uint32_t I2C_FREQUENCY_HZ = 400000;

    //Adafruit breakout default is 0x18, Sparkfun default is 0x19
    constexpr uint8_t ACCEL_I2C_ADDRESS = 0x19;

    //Select which axis points toward the robot center (LIS331::X_AXIS = X, LIS331::Y_AXIS = Y, LIS331::Z_AXIS = Z)
    constexpr LIS331::int_axis ACCEL_FORCE_AXIS = LIS331::Z_AXIS;

}  // namespace AccelConfig

namespace DebugConfig {

    constexpr bool ENABLE_RPM_HISTORY = true;
    constexpr uint16_t RPM_HISTORY_SIZE = 60;
    constexpr uint32_t RPM_HISTORY_LOG_INTERVAL_MS = 50;

}  // namespace DebugConfig

namespace HomerConfig {

    //----------TRANSLATIONAL DRIFT SETTINGS----------
    constexpr float ACCEL_MOUNT_RADIUS_CM = 5.2f;              //Radius of accelerometer from center of robot                      
    constexpr float DEFAULT_ACCEL_ZERO_G_OFFSET = 3.7f;       //Offset value (in G) used to get 0G accelerometer reading when robot is at rest, For small-radius bots openmelt recommends trying H3LIS331 at +/-200g range (MED_RANGE) for improved accuracy (accel_handler.h)
    constexpr float BASE_SPIN_MAX_OFFSET_PERCENTAGE = 0.4f;    //Offset percentage value (0.0f - 1.0f) to limit base spin speed below full ESC range so translation modulation still has headroom at high throttle

    //May need this later
    constexpr uint16_t MIN_TRANSLATION_RPM = 400;   //full power spin in below this number (increasing can reduce spin-up time)

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

namespace MotorConfig {

    constexpr uint16_t ESC_MIN_US = 1000;      // Minimum pulse width for ESC (full reverse in microseconds)
    constexpr uint16_t ESC_NEUTRAL_US = 1500;  // Neutral pulse width for ESC (stop in microseconds)
    constexpr uint16_t ESC_MAX_US = 2000;      // Maximum pulse width for ESC (full forward in microseconds)
    constexpr float ESC_RANGE_US = static_cast<float>(ESC_MAX_US - ESC_NEUTRAL_US); // From neutral to max in one direction (500 us)

    constexpr uint32_t ESC_PWM_FREQUENCY_HZ = 50;   // Increase for faster ESC command updates and potentially smoother response, Higher values may reduce compatibility with some ESCs
    constexpr uint32_t ESC_FRAME_US = 1000000UL / ESC_PWM_FREQUENCY_HZ;  // Frame width in microseconds based on configured frequency (e.g. 20000 us for 50 Hz)
    constexpr uint8_t ESC_PWM_RESOLUTION_BITS = 12; // PWM duty resolution, Higher values allow finer PWM pulse precision but very high resolutions may limit maximum achievable PWM frequency
    constexpr uint32_t ESC_PWM_MAX_DUTY = (1UL << ESC_PWM_RESOLUTION_BITS) - 1UL;   // Maximum duty cycle value based on configured PWM resolution (e.g. 4095 for 12-bit resolution)

    constexpr uint8_t MOTOR_1_PWM_CHANNEL = 0;  // LEDC channel slop
    constexpr uint8_t MOTOR_2_PWM_CHANNEL = 1;  // LEDC channel slop

}  // namespace MotorConfig

namespace MovementConfig {

    // Drive Mode Options: DRIVE_MANUAL, DRIVE_DIFF, DRIVE_MELTYBRAIN
    enum DriveMode {
    MANUAL,
    DIFF_DRIVE,
    MELTYBRAIN
    };

    constexpr DriveMode DRIVE_MODE = DIFF_DRIVE; // Variable used to determine movement code in movement_control.cpp

    constexpr float THROTTLE_DEADZONE = 0.03f;          // Deadzone for channel ch3 (Throttle) to prevent noise around neutral from causing movement
    constexpr float TRANS_VECTOR_DEADZONE = 0.02f;      // Deadzone for ch1 and ch2 (Translation) to prevent noise around neutral from causing movement
    constexpr float DIFF_TRANS_VECTOR_DEADZONE = 0.2f;  // Diff Drive Deadzone for ch1 and ch2, mostly for preventing turning when trying to go straight

    struct RpmLookupEntry {
    uint16_t pwm_us;
    float rpm;
    };

    constexpr RpmLookupEntry RPM_LOOKUP[] = {
        {1500,    0.0f},
        {1550,  512.5f},
        {1575, 1065.6f},
        {1600, 1530.4f},
        {1625, 1943.9f},
        {1650, 2112.2f},
    };

}  // namespace MovementConfig

namespace RcConfig {

    constexpr uint16_t RC_MIN_US = 1000;      // Minimum clamp value for RC input pulse width (in microseconds)
    constexpr uint16_t RC_NEUTRAL_US = 1500;  // Neutral pulse width for RC input (in microseconds)
    constexpr uint16_t RC_MAX_US = 2000;      // Maximum clamp value for RC input pulse width (in microseconds)
    constexpr float RC_RANGE_US = static_cast<float>(RC_MAX_US - RC_NEUTRAL_US); // From neutral to max in one direction (500 us)

    constexpr uint16_t RC_ISR_MIN_US = 800;   // Minimum valid pulse width for RC signal (in microseconds)
    constexpr uint16_t RC_ISR_MAX_US = 2200;  // Maximum valid pulse width for RC signal (in microseconds)

    constexpr uint32_t RC_SIGNAL_LOST_TIMEOUT_US = 100000;  // Timeout for RC signal loss detection (in microseconds)

}  // namespace RcConfig