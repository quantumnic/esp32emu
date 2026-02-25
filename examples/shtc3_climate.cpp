// esp32emu example — SHTC3 humidity & temperature sensor
#include <Arduino.h>
#include <Adafruit_SHTC3.h>

Adafruit_SHTC3 shtc3;

void setup() {
    Serial.begin(115200);
    if (!shtc3.begin()) {
        Serial.println("SHTC3 not found!");
        while (1) delay(10);
    }
    Serial.println("SHTC3 climate sensor ready");
}

void loop() {
    sensors_event_t_shtc3 humidity, temp;
    shtc3.getEvent(&humidity, &temp);

    Serial.printf("Temperature: %.1f °C, Humidity: %.1f %%\n",
                  temp.temperature, humidity.relative_humidity);

    // Compute heat index approximation
    float hi = temp.temperature + 0.33f * (humidity.relative_humidity / 100.0f * 6.105f *
               exp(17.27f * temp.temperature / (237.7f + temp.temperature))) - 4.0f;
    Serial.printf("Heat index: %.1f °C\n", hi);

    delay(2000);
}
