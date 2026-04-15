#include <Arduino.h>

#define ACCLOCK  D0
#define ACCATA D1
#define MOTOR_1 D5 // Right motor if weapon facing forward
#define MOTOR_2 D6 // Left motor if weapon facing forward
#define CHAN_1 D7
#define CHAN_2 D8
#define CHAN_3 D9
#define CHAN_4 D10

volatile uint32_t riseTime = 0;
volatile uint32_t pulseWidth = 1500; // default neutral
volatile bool newPulse = false;
volatile uint32_t lastPulseMicros = 0;

static const uint32_t NEUTRAL_US = 1500;
static const uint32_t MIN_US = 1000;
static const uint32_t MAX_US = 2000;
static const uint32_t FAILSAFE_US = NEUTRAL_US;
static const uint32_t FAILSAFE_TIMEOUT_US = 100000; // 100ms

void IRAM_ATTR handleInterrupt() {
    if (digitalRead(CHAN_3) == HIGH) {
        riseTime = micros();
    } else {
        uint32_t now = micros();
        pulseWidth = now - riseTime;
        newPulse = true;
        lastPulseMicros = now;
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(CHAN_1, INPUT);
    pinMode(CHAN_2, INPUT);
    pinMode(CHAN_3, INPUT);
    pinMode(CHAN_4, INPUT);
    pinMode(MOTOR_1, OUTPUT);
    pinMode(MOTOR_2, OUTPUT);
    digitalWrite(MOTOR_1, LOW);
    digitalWrite(MOTOR_2, LOW);

    attachInterrupt(digitalPinToInterrupt(CHAN_3), handleInterrupt, CHANGE);
}

void loop() {
    static uint32_t lastPrint = 0;
    uint32_t localPulse = NEUTRAL_US;
    uint32_t localLastPulseMicros = 0;

    // Print at ~20Hz to avoid spamming serial
    if (newPulse && millis() - lastPrint > 50) {
        Serial.print("PWM: ");
        Serial.println(pulseWidth);
        lastPrint = millis();
        newPulse = false;
    }

    // Snapshot shared values safely
    noInterrupts();
    localPulse = pulseWidth;
    localLastPulseMicros = lastPulseMicros;
    interrupts();

    // Clamp pulse to a safe range
    if (localPulse < MIN_US) localPulse = MIN_US;
    if (localPulse > MAX_US) localPulse = MAX_US;

    // Failsafe to neutral if input signal is lost
    if (micros() - localLastPulseMicros > FAILSAFE_TIMEOUT_US) {
        localPulse = FAILSAFE_US;
    }

    // Generate PWM output (50Hz frame)
    digitalWrite(MOTOR_1, HIGH);
    delayMicroseconds(localPulse);
    digitalWrite(MOTOR_1, LOW);

    delayMicroseconds(20000 - localPulse); // complete 20ms period
}
