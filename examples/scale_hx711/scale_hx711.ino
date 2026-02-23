// Digital Scale with NAU7802 ADC
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NAU7802.h>

Adafruit_NAU7802 nau;

float calibrationFactor = 420.0; // counts per gram
int32_t tareOffset = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!nau.begin()) {
        Serial.println("NAU7802 not found!");
        while (true) delay(1000);
    }

    nau.setLDO(4);     // 3.3V
    nau.setGain(128);   // Max gain for load cell
    nau.setRate(80);    // 80 SPS

    nau.calibrate(0);
    nau.waitForCalibration();

    // Tare: average 10 readings
    Serial.println("Taring... keep scale empty");
    delay(1000);
    int32_t sum = 0;
    for (int i = 0; i < 10; i++) {
        while (!nau.available()) delay(1);
        sum += nau.read();
    }
    tareOffset = sum / 10;
    Serial.print("Tare offset: ");
    Serial.println(tareOffset);
    Serial.println("Scale ready! Place items to weigh.");
}

void loop() {
    if (nau.available()) {
        int32_t raw = nau.read();
        float grams = (raw - tareOffset) / calibrationFactor;
        Serial.print("Weight: ");
        Serial.print(grams, 1);
        Serial.println(" g");
    }
    delay(100);
}
