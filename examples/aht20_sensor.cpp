#include <Arduino.h>
#include <AHTxx.h>

AHT20 aht;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    if (!aht.begin()) {
        Serial.println("AHT20 not found!");
        return;
    }
    Serial.println("AHT20 Temperature & Humidity Sensor ready");
}

void loop() {
    float temp = aht.readTemperature();
    float hum = aht.readHumidity();

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.println(" %");
    Serial.println();
    delay(2000);
}
