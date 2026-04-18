#include <Arduino.h>

#include "melty_config.h"
#include "motor_driver.h"
#include "rc_handler.h"
#include "spin_control.h"

static uint16_t read_ch1() { return rc_get_ch1_pulse_us(); }
static uint16_t read_ch2() { return rc_get_ch2_pulse_us(); }
static uint16_t read_ch3() { return rc_get_ch3_pulse_us(); }
static uint16_t read_ch4() { return rc_get_ch4_pulse_us(); }

static bool throttle_is_active() {
  int ch3_centered = (int)read_ch3() - (int)RC_NEUTRAL_US;
  int ch2_centered = (int)read_ch2() - (int)RC_NEUTRAL_US;
  return abs(ch3_centered) > 10 || abs(ch2_centered) > 10;
}

static void echo_diagnostics() {
  Serial.print("  RC Health: ");
  Serial.print(rc_signal_is_healthy());
  Serial.print("  CH1: ");
  Serial.print(read_ch1());
  Serial.print("  CH2: ");
  Serial.print(read_ch2());
  Serial.print("  CH3: ");
  Serial.print(read_ch3());
  Serial.print("  CH4: ");
  Serial.print(read_ch4());
  Serial.print("  Max RPM: ");
  Serial.print(get_max_rpm());
  Serial.println();
}

static void wait_for_rc_and_neutral_throttle() {
  while (!rc_signal_is_healthy() || throttle_is_active()) {
    motors_off();
    delay(300);
    echo_diagnostics();
  }
}

static void handle_idle() {
  motors_off();
  delay(150);
  echo_diagnostics();
}

void setup() {
  Serial.begin(115200);

  pinMode(HEADING_LED_PIN, OUTPUT);
  digitalWrite(HEADING_LED_PIN, HIGH);

  init_motors();
  init_rc();

#ifdef VERIFY_RC_THROTTLE_ZERO_AT_BOOT
  wait_for_rc_and_neutral_throttle();
#endif
}

void loop() {
  if (!rc_signal_is_healthy()) {
    motors_off();
    delay(300);
    echo_diagnostics();
    return;
  }

  if (throttle_is_active()) {
    spin_one_rotation();
    // drive_both_motors_from_ch3();
    echo_diagnostics();
  } else {
    handle_idle();
  }
}
