// TSL2561 ambient light monitor example
// Reads lux values and prints light level classification

#include <Arduino.h>
#include <Adafruit_TSL2561.h>

Adafruit_TSL2561_Unified tsl(TSL2561_ADDR_FLOAT, 2561);

void setup() {
    Serial.begin(115200);
    Serial.println("TSL2561 Light Monitor");

    if (!tsl.begin()) {
        Serial.println("ERROR: TSL2561 not found!");
        while (1) delay(100);
    }

    tsl.enableAutoRange(true);
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
    tsl.setGain(TSL2561_GAIN_1X);

    // Inject test data for emulation
    tsl.setTestLux(250.0f);

    Serial.println("Sensor configured.");
}

void loop() {
    Adafruit_TSL2561_Unified::sensor_event_t event;
    if (tsl.getEvent(&event)) {
        Serial.print("Light: ");
        Serial.print(event.light);
        Serial.print(" lux — ");

        if (event.light < 10)
            Serial.println("🌙 Very dark");
        else if (event.light < 50)
            Serial.println("🕯️ Dim");
        else if (event.light < 200)
            Serial.println("💡 Indoor");
        else if (event.light < 1000)
            Serial.println("☀️ Bright indoor");
        else
            Serial.println("🌞 Outdoor");
    } else {
        Serial.println("Sensor read failed!");
    }

    delay(1000);
}
