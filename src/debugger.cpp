#include <Arduino.h>
#include <Preferences.h>

#include "debugger.h"
#include "movement_control.h"

static Preferences prefs;

void init_debug_logger() {
    prefs.begin("rpm_log", false);
}

void save_rpm_debug_log_to_flash() {

    const RpmDebugLog& log = get_rpm_debug_log();

    prefs.putUShort("count", log.count);
    prefs.putBool("wrapped", log.wrapped);
    prefs.putUShort("next", log.next_index);
    prefs.putFloat("max", log.max_rpm);

    for (uint16_t i = 0; i < log.count; i++) {

        uint16_t physical_index = i;

        if (log.wrapped) {
            physical_index = (log.next_index + i) % DebugConfig::RPM_HISTORY_SIZE;
        }

        const RpmDebugSample& sample = log.samples[physical_index];

        char key[16];

        snprintf(key, sizeof(key), "t%03u", i);
        prefs.putUInt(key, sample.time_ms);

        snprintf(key, sizeof(key), "p%03u", i);
        prefs.putUShort(key, sample.throttle_us);

        snprintf(key, sizeof(key), "n%03u", i);
        prefs.putFloat(key, sample.throttle_normalized);

        snprintf(key, sizeof(key), "g%03u", i);
        prefs.putFloat(key, sample.accel_g);

        snprintf(key, sizeof(key), "r%03u", i);
        prefs.putFloat(key, sample.rpm);
    }
}

void print_saved_rpm_debug_log_from_flash() {

    uint16_t count = prefs.getUShort("count", 0);
    float max_rpm = prefs.getFloat("max", 0.0f);

    if (count == 0) {
        Serial.println("NO_SAVED_RPM_DEBUG_LOG");
        return;
    }

    Serial.println("RPM_DEBUG_LOG_BEGIN");

    Serial.print("MAX_RPM: ");
    Serial.println(max_rpm);

    Serial.println("time_ms,throttle_us,throttle_norm,accel_g,rpm");

    for (uint16_t i = 0; i < count; i++) {

        char key[16];

        snprintf(key, sizeof(key), "t%03u", i);
        uint32_t time_ms = prefs.getUInt(key, 0);

        snprintf(key, sizeof(key), "p%03u", i);
        uint16_t throttle_us = prefs.getUShort(key, 0);

        snprintf(key, sizeof(key), "n%03u", i);
        float throttle_norm = prefs.getFloat(key, 0.0f);

        snprintf(key, sizeof(key), "g%03u", i);
        float accel_g = prefs.getFloat(key, 0.0f);

        snprintf(key, sizeof(key), "r%03u", i);
        float rpm = prefs.getFloat(key, 0.0f);

        Serial.print(time_ms);
        Serial.print(",");
        Serial.print(throttle_us);
        Serial.print(",");
        Serial.print(throttle_norm, 3);
        Serial.print(",");
        Serial.print(accel_g, 4);
        Serial.print(",");
        Serial.println(rpm, 2);
    }

    Serial.println("RPM_DEBUG_LOG_END");
}

void clear_saved_rpm_debug_log() {
    prefs.clear();
}