#include <Arduino.h>
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

void setup() {
    Serial.begin(115200);
    if (!aht.begin()) {
        Serial.println("AHTX0 not found!");
        return;
    }
    Serial.println("AHTX0 Temperature & Humidity Sensor ready");
}

void loop() {
    Adafruit_AHTX0::sensors_event_t humidity, temp;
    if (aht.getEvent(&humidity, &temp)) {
        Serial.print("Temperature: ");
        Serial.print(temp.temperature);
        Serial.println(" °C");
        Serial.print("Humidity: ");
        Serial.print(humidity.relative_humidity);
        Serial.println(" %RH");
    } else {
        Serial.println("Read failed!");
    }
    Serial.println("---");
    delay(2000);
}
