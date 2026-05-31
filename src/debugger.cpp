#include "debugger.h"
#include <Arduino.h>
#include <Preferences.h>
#include <Wire.h>
#include "homer_config.h"
#include "accel_handler.h"
#include "movement_control.h"
#include "rc_handler.h"

static float rpm_history[DebugConfig::RPM_HISTORY_SIZE];
static uint16_t throttle_history[DebugConfig::RPM_HISTORY_SIZE];
static uint16_t rpm_history_index = 0;
static bool rpm_history_wrapped = false;

static Preferences prefs;

void init_debug_logger() {
    prefs.begin("rpm_log", false);
}

void log_rpm_history(float rpm, uint16_t throttle) {

    static uint32_t last_log_ms = 0;
    uint32_t now_ms = millis();

    if (now_ms - last_log_ms < DebugConfig::RPM_HISTORY_LOG_INTERVAL_MS) {
        return;
    }

    last_log_ms = now_ms;

    rpm_history[rpm_history_index] = rpm;
    throttle_history[rpm_history_index] = throttle;

    rpm_history_index++;

    if (rpm_history_index >= DebugConfig::RPM_HISTORY_SIZE) {
        rpm_history_index = 0;
        rpm_history_wrapped = true;
    }
}

void save_rpm_history_to_flash() {

    uint16_t count = rpm_history_wrapped ? DebugConfig::RPM_HISTORY_SIZE : rpm_history_index;

    prefs.putUShort("count", count);
    prefs.putBool("wrapped", rpm_history_wrapped);
    prefs.putUShort("index", rpm_history_index);

    for (uint16_t i = 0; i < count; i++) {

        uint16_t physical_index = i;

        if (rpm_history_wrapped) {
            physical_index =
                (rpm_history_index + i) %
                DebugConfig::RPM_HISTORY_SIZE;
        }

        char key[16];

        snprintf(key, sizeof(key), "r%03u", i);
        prefs.putFloat(key, rpm_history[physical_index]);

        snprintf(key, sizeof(key), "t%03u", i);
        prefs.putFloat(key, throttle_history[physical_index]);
    }
}

void clear_saved_rpm_history() {
    prefs.clear();
}

void print_saved_rpm_history_from_flash() {

    uint16_t count = prefs.getUShort("count", 0);

    if (count == 0) {
        Serial.println("NO_SAVED_RPM_HISTORY");
        return;
    }

    Serial.println("RPM_HISTORY_BEGIN");
    Serial.println("throttle,rpm");

    for (uint16_t i = 0; i < count; i++) {

        char key[16];

        snprintf(key, sizeof(key), "t%03u", i);
        uint16_t throttle = prefs.getUShort(key, 0);

        snprintf(key, sizeof(key), "r%03u", i);
        float rpm = prefs.getFloat(key, 0.0f);

        Serial.print(throttle);
        Serial.print(",");
        Serial.println(rpm, 2);
    }

    Serial.println("RPM_HISTORY_END");
}

void scan_i2c() {

	Serial.println("Scanning I2C...");

    uint8_t count = 0;

    for (uint8_t address = 1; address < 127; address++) {

        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at 0x");

            if (address < 16) {
                Serial.print("0");
            }

            Serial.println(address, HEX);

            count++;
        }
    }

    if (count == 0) {
        Serial.println("No I2C devices found");
    }

    Serial.println("Scan done");
}

void echo_diagnostics(const RcInput& input, const TranslationVector& translation_vector, const SpinCommand& spin_command) {

    Serial.print("  RC Health: ");
    Serial.print(input.healthy);
    Serial.print("  CH1: ");
    Serial.print(input.ch1_us);
    Serial.print("  CH2: ");
    Serial.print(input.ch2_us);
    Serial.print("  CH3: ");
    Serial.print(input.ch3_us);
    Serial.print("  CH4: ");
    Serial.print(input.ch4_us);

    Serial.print("  TVec X: ");
    Serial.print(translation_vector.x, 3);
    Serial.print("  TVec Y: ");
    Serial.print(translation_vector.y, 3);
    Serial.print("  TVec Mag: ");
    Serial.print(translation_vector.magnitude, 3);
    Serial.print("  TVec Angle: ");
    Serial.print(translation_vector.angle_deg, 1);

    Serial.print("  Spin Throttle: ");
    Serial.print(spin_command.throttle, 3);
    Serial.print("  Spin Active: ");
    Serial.print(spin_command.active);
    Serial.println();

}