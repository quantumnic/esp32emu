#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm;

#define SERVOMIN  150  // Minimum pulse length count (out of 4096)
#define SERVOMAX  600  // Maximum pulse length count (out of 4096)
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz

void setup() {
    Serial.begin(115200);
    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ);
    Serial.println("PCA9685 16-Channel Servo Driver ready");
}

void loop() {
    // Sweep servo on channel 0
    for (uint16_t pulse = SERVOMIN; pulse < SERVOMAX; pulse += 10) {
        pwm.setPWM(0, 0, pulse);
        delay(20);
    }
    Serial.println("Sweep forward complete");

    for (uint16_t pulse = SERVOMAX; pulse > SERVOMIN; pulse -= 10) {
        pwm.setPWM(0, 0, pulse);
        delay(20);
    }
    Serial.println("Sweep reverse complete");

    // Use writeMicroseconds for channel 1
    pwm.writeMicroseconds(1, 1500); // center
    delay(500);
    pwm.writeMicroseconds(1, 1000); // min
    delay(500);
    pwm.writeMicroseconds(1, 2000); // max
    delay(500);
    Serial.println("Microsecond control complete");
}
