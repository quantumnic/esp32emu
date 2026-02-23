// esp32emu example: PCA9685 Multi-Servo Controller
// Controls multiple servos via PCA9685 16-channel PWM driver.

#include <Arduino.h>
#include <Wire.h>
#include <PCA9685.h>

PCA9685 pwm(0x40);

#define SERVO_MIN 150  // ~0 degrees
#define SERVO_MAX 600  // ~180 degrees

uint16_t angleToPulse(int angle) {
    return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void setup() {
    Serial.begin(115200);
    Wire.begin();

    pwm.begin();
    pwm.setPWMFreq(50); // 50 Hz for servos
    pwm.setOscillatorFrequency(27000000);

    Serial.println("PCA9685 Servo Controller");
    Serial.println("========================");

    // Center all servos
    for (int ch = 0; ch < 4; ch++) {
        pwm.setPWM(ch, 0, angleToPulse(90));
    }
    Serial.println("All servos centered at 90°");
}

void loop() {
    // Sweep servo 0 from 0 to 180
    for (int angle = 0; angle <= 180; angle += 10) {
        pwm.setPWM(0, 0, angleToPulse(angle));
        Serial.printf("Servo 0: %d°\n", angle);
        delay(100);
    }

    // Move servo 1 with writeMicroseconds
    pwm.writeMicroseconds(1, 1500); // center
    Serial.println("Servo 1: center (1500us)");
    delay(500);

    pwm.writeMicroseconds(1, 2000); // max
    Serial.println("Servo 1: max (2000us)");
    delay(500);

    delay(1000);
}
