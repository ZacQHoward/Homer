#include <Arduino.h>

#include "melty_config.h"
#include "accel_handler.h"

static float calibrated_zero_g_offset = DEFAULT_ACCEL_ZERO_G_OFFSET;

void setup() {
    Serial.begin(115200);
    delay(500);
    init_accel();
    Serial.println("Accel test started");

    // // Take a short resting sample set to estimate the zero-g offset.
    // const int sample_count = 1000;
    // float sum = 0.0f;
    // for (int i = 0; i < sample_count; i++) {
    //     sum += get_accel_force_g();
    //     delay(10);
    // }

    // calibrated_zero_g_offset = sum / sample_count;
    // Serial.print("Suggested DEFAULT_ACCEL_ZERO_G_OFFSET: ");
    // Serial.println(calibrated_zero_g_offset, 4);
}

void loop() {
    int16_t x, y, z;
    get_accel_axes(x, y, z);
    float raw_g = get_accel_force_g();
    float corrected_g = fabs(raw_g - DEFAULT_ACCEL_ZERO_G_OFFSET);

    Serial.print("Accel Raw G: ");
    Serial.print(raw_g, 4);
    Serial.print("  Zeroed G: ");
    Serial.print(corrected_g, 4);
    Serial.print("  Raw X/Y/Z: ");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.println(z);
    delay(100);
}
