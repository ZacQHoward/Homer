#include <Arduino.h>

#include "melty_config.h"
#include "accel_handler.h"
#include "rc_handler.h"

#ifdef ENABLE_WIFI_DASHBOARD
#include "wifi_server.h"
#endif

static float calibrated_zero_g_offset = DEFAULT_ACCEL_ZERO_G_OFFSET;
static unsigned long last_sample_ms = 0;
static unsigned long avg_window_start_ms = 0;
static float avg_window_sum_zeroed = 0.0f;
static unsigned int avg_window_count = 0;
static float avg_zeroed_g_30s = 0.0f;

#ifdef ENABLE_WIFI_DASHBOARD
static const char *AP_SSID = "Homer-Test";
static const char *AP_PASSWORD = "12345678";
#endif

void setup() {
    Serial.begin(115200);
    delay(500);
    init_accel();
    init_rc();
    Serial.println("Accel test started");
#ifdef ENABLE_WIFI_DASHBOARD
    init_wifi_dashboard(AP_SSID, AP_PASSWORD);
#endif
    avg_window_start_ms = millis();
}

void loop() {
#ifdef ENABLE_WIFI_DASHBOARD
    wifi_dashboard_loop();
#endif

    if (millis() - last_sample_ms >= 100) {
        last_sample_ms = millis();

#ifdef ENABLE_WIFI_DASHBOARD
        telemetry_data_t data = {};
        data.sample_ms = millis();
        get_accel_axes(data.x, data.y, data.z);
        data.raw_g = get_accel_force_g();
        data.zeroed_g = fabs(data.raw_g - calibrated_zero_g_offset);
        data.avg_zeroed_g = avg_zeroed_g_30s;
        data.offset = calibrated_zero_g_offset;
        data.ch1_us = rc_get_ch1_pulse_us();
        data.ch2_us = rc_get_ch2_pulse_us();
        data.ch3_us = rc_get_ch3_pulse_us();
        data.ch4_us = rc_get_ch4_pulse_us();
        data.controller_connected = rc_signal_is_healthy();
        data.rc_healthy = rc_signal_is_healthy();
        wifi_dashboard_set_data(data);
        float current_zeroed_g = data.zeroed_g;
        float current_raw_g = data.raw_g;
        int16_t current_x = data.x;
        int16_t current_y = data.y;
        int16_t current_z = data.z;
        uint16_t ch1 = data.ch1_us;
        uint16_t ch2 = data.ch2_us;
        uint16_t ch3 = data.ch3_us;
        uint16_t ch4 = data.ch4_us;
        bool rc_connected = data.controller_connected;
        bool rc_healthy = data.rc_healthy;
#else
        int16_t current_x = 0;
        int16_t current_y = 0;
        int16_t current_z = 0;
        get_accel_axes(current_x, current_y, current_z);
        float current_raw_g = get_accel_force_g();
        float current_zeroed_g = fabs(current_raw_g - calibrated_zero_g_offset);
        uint16_t ch1 = rc_get_ch1_pulse_us();
        uint16_t ch2 = rc_get_ch2_pulse_us();
        uint16_t ch3 = rc_get_ch3_pulse_us();
        uint16_t ch4 = rc_get_ch4_pulse_us();
        bool rc_connected = rc_signal_is_healthy();
        bool rc_healthy = rc_signal_is_healthy();
#endif

        avg_window_sum_zeroed += current_zeroed_g;
        avg_window_count++;

        if (millis() - avg_window_start_ms >= 30000UL) {
            if (avg_window_count > 0) {
                avg_zeroed_g_30s = avg_window_sum_zeroed / avg_window_count;
                Serial.print("30s Avg Zeroed G: ");
                Serial.println(avg_zeroed_g_30s, 4);
            }

            avg_window_start_ms = millis();
            avg_window_sum_zeroed = 0.0f;
            avg_window_count = 0;
        }

        Serial.print("Accel Raw G: ");
        Serial.print(current_raw_g, 4);
        Serial.print("  Zeroed G: ");
        Serial.print(current_zeroed_g, 4);
        Serial.print("  Avg Zeroed G (30s): ");
        Serial.print(avg_zeroed_g_30s, 4);
        Serial.print("  RC(us) CH1/CH2/CH3/CH4: ");
        Serial.print(ch1);
        Serial.print(", ");
        Serial.print(ch2);
        Serial.print(", ");
        Serial.print(ch3);
        Serial.print(", ");
        Serial.print(ch4);
        Serial.print("  Controller Connected: ");
        Serial.print(rc_connected);
        Serial.print("  RC Health: ");
        Serial.print(rc_healthy);
        Serial.print("  Raw X/Y/Z: ");
        Serial.print(current_x);
        Serial.print(", ");
        Serial.print(current_y);
        Serial.print(", ");
        Serial.println(current_z);
    }
}
