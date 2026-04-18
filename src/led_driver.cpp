#include "arduino.h"
#include "melty_config.h"
#include "led_driver.h"

void init_led(void) {
  pinMode(HEADING_LED_PIN, OUTPUT);
  digitalWrite(HEADING_LED_PIN, LOW);
}

void heading_led_on(int shimmer) {
  if (shimmer == 1) {
    if (micros() & (1 << 10)) {
      digitalWrite(HEADING_LED_PIN, HIGH);
    } else {
      digitalWrite(HEADING_LED_PIN, LOW);
    }
  } else {
    digitalWrite(HEADING_LED_PIN, HIGH);
  }
}

void heading_led_off() {
  digitalWrite(HEADING_LED_PIN, LOW);
}
