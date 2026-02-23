// VCNL4040 proximity + ambient light sensor example
// Gesture detection and light level monitoring

#include <Arduino.h>
#include <Adafruit_VCNL4040.h>

Adafruit_VCNL4040 vcnl;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!vcnl.begin()) {
        Serial.println("VCNL4040 not found!");
        while (1) delay(10);
    }

    vcnl.enableProximity(true);
    vcnl.enableAmbientLight(true);
    vcnl.setProximityIntegrationTime(Adafruit_VCNL4040::VCNL4040_PS_IT_4T);
    vcnl.setAmbientIntegrationTime(Adafruit_VCNL4040::VCNL4040_ALS_IT_160MS);
    vcnl.setProximityHighResolution(true);

    // Set interrupt thresholds
    vcnl.setProximityLowThreshold(50);
    vcnl.setProximityHighThreshold(500);

    Serial.println("VCNL4040 Proximity & Light Sensor");
    Serial.println("===================================");

    // Inject test values
    vcnl.test_setProximity(150);
    vcnl.test_setLux(420);
    vcnl.test_setWhiteLight(380);
}

void loop() {
    uint16_t prox = vcnl.getProximity();
    uint16_t lux = vcnl.getLux();
    uint16_t white = vcnl.getWhiteLight();

    Serial.print("Proximity: ");
    Serial.print(prox);
    Serial.print("  |  Lux: ");
    Serial.print(lux);
    Serial.print("  |  White: ");
    Serial.println(white);

    if (prox > 500) {
        Serial.println("  👋 Object very close!");
    } else if (prox > 100) {
        Serial.println("  🤚 Object nearby");
    }

    delay(200);
}
