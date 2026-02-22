// HTU21D-F humidity & temperature monitor
#include <Wire.h>
#include <Adafruit_HTU21DF.h>

Adafruit_HTU21DF htu;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!htu.begin()) {
        Serial.println("HTU21D-F not found!");
        while (1) delay(1000);
    }
    Serial.println("HTU21D-F initialized");
}

void loop() {
    float temp = htu.readTemperature();
    float hum = htu.readHumidity();

    Serial.print("Temperature: ");
    Serial.print(temp, 1);
    Serial.print(" °C  Humidity: ");
    Serial.print(hum, 1);
    Serial.println(" %");

    delay(2000);
}
