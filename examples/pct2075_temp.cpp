#include <Arduino.h>
#include <Adafruit_PCT2075.h>

Adafruit_PCT2075 pct;

void setup() {
    Serial.begin(115200);
    Serial.println("PCT2075 Temperature Sensor");

    if (!pct.begin()) {
        Serial.println("Failed to find PCT2075!");
        while (1) delay(10);
    }

    pct.setHighTemperatureThreshold(80.0);
    pct.setTemperatureHysteresis(75.0);
    pct.setFaultCount(4);
    Serial.println("PCT2075 initialized");
}

void loop() {
    float temp = pct.getTemperature();
    Serial.print("Temperature: ");
    Serial.print(temp, 1);
    Serial.println(" °C");

    if (temp > pct.getHighTemperatureThreshold()) {
        Serial.println("🔥 OVERTEMP!");
    }

    delay(1000);
}
