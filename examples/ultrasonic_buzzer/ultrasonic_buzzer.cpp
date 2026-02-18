// Ultrasonic Distance Sensor + Buzzer Example
// Demonstrates: pulseIn(), tone(), noTone(), GPIO
// Board: Arduino Uno or ESP32

#include "Arduino.h"

#define TRIG_PIN  9
#define ECHO_PIN  10
#define BUZZER_PIN 8
#define LED_PIN    13

float measureDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    unsigned long duration = pulseIn(ECHO_PIN, HIGH);
    return duration * 0.034 / 2.0;  // cm
}

void setup() {
    Serial.begin(9600);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

    Serial.println("Ultrasonic Parking Sensor");
    Serial.println("========================");
}

void loop() {
    static int readings = 0;

    float distance = measureDistance();

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Beep faster as object gets closer
    if (distance < 10) {
        tone(BUZZER_PIN, 2000);  // continuous
        digitalWrite(LED_PIN, HIGH);
        Serial.println("⚠️  VERY CLOSE!");
    } else if (distance < 30) {
        tone(BUZZER_PIN, 1000, 100);
        digitalWrite(LED_PIN, HIGH);
    } else {
        noTone(BUZZER_PIN);
        digitalWrite(LED_PIN, LOW);
    }

    readings++;
    if (readings >= 5) {
        noTone(BUZZER_PIN);
        Serial.println("\nDemo complete!");
        while(1) delay(1000);
    }

    delay(200);
}
