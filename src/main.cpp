// Open Melt controller entry point, trimmed for a PlatformIO project.
// EEPROM and battery monitoring are disabled for the first XIAO ESP32C3 port.

#include <Arduino.h>

#include "rc_handler.h"
#include "melty_config.h"
#include "motor_driver.h"
#include "accel_handler.h"
#include "spin_control.h"
#include "led_driver.h"

#ifdef ENABLE_WATCHDOG
#include <Adafruit_SleepyDog.h>
#endif

static void echo_diagnostics();

static void service_watchdog() {
#ifdef ENABLE_WATCHDOG
    Watchdog.reset();
#endif
}

// Loops until a good RC signal is detected and throttle is zero.
static void wait_for_rc_good_and_zero_throttle() {
    while (rc_signal_is_healthy() == false || rc_get_throttle_percent() > 0) {
        heading_led_on(0);
        delay(250);
        heading_led_off();
        delay(250);

        service_watchdog();
        echo_diagnostics();
    }
}

// Dumps diagnostics over USB serial.
static void echo_diagnostics() {
    Serial.print("Raw Accel G: ");
    Serial.print(get_accel_force_g());
    Serial.print("  RC Health: ");
    Serial.print(rc_signal_is_healthy());
    Serial.print("  RC Throttle: ");
    Serial.print(rc_get_throttle_percent());
    Serial.print("  RC L/R: ");
    Serial.print(rc_get_leftright());
    Serial.print("  RC F/B: ");
    Serial.print(rc_get_forback());
    Serial.println("");
}

// Used to flash out max recorded RPM in hundreds.
static void display_rpm_if_requested() {
    if (rc_get_forback() == RC_FORBACK_FORWARD) {
        delay(750);
        if (rc_get_forback() == RC_FORBACK_FORWARD && rc_get_throttle_percent() == 0) {
            for (int x = 0; x < get_max_rpm() && rc_get_throttle_percent() == 0; x = x + 100) {
                service_watchdog();
                delay(600);
                heading_led_on(0);
                delay(20);
                heading_led_off();
            }
            delay(1500);
        }
    }
}

// Checks if user has requested config mode.
static void check_config_mode() {
    if (rc_get_forback() == RC_FORBACK_BACKWARD) {
        delay(750);
        if (rc_get_forback() == RC_FORBACK_BACKWARD) {
            toggle_config_mode();
            if (get_config_mode() == false) {
                save_melty_config_settings();
            }

            while (rc_get_forback() == RC_FORBACK_BACKWARD) {
                service_watchdog();
            }
        }
    }
}

// Handles the bot when not spinning.
static void handle_bot_idle() {
    motors_off();

    heading_led_on(0);
    delay(30);
    heading_led_off();
    delay(120);

    if (get_config_mode() == true) {
        heading_led_off();
        delay(400);
        heading_led_on(0);
        delay(30);
        heading_led_off();
        delay(140);
    }

    check_config_mode();
    display_rpm_if_requested();
    echo_diagnostics();
}

void setup() {
    Serial.begin(115200);

    init_motors();
    init_led();

#ifdef ENABLE_WATCHDOG
    Watchdog.enable(WATCH_DOG_TIMEOUT_MS);
#endif

    init_rc();
    init_accel();

#ifdef VERIFY_RC_THROTTLE_ZERO_AT_BOOT
    wait_for_rc_good_and_zero_throttle();
    delay(MAX_MS_BETWEEN_RC_UPDATES + 1);
    wait_for_rc_good_and_zero_throttle();
#endif
}

void loop() {
    service_watchdog();

    while (rc_signal_is_healthy() == false) {
        motors_off();

        heading_led_on(0);
        delay(30);
        heading_led_off();
        delay(600);

        service_watchdog();
        echo_diagnostics();
    }

    if (rc_get_throttle_percent() > 0) {
        spin_one_rotation();
    } else {
        handle_bot_idle();
    }
}
