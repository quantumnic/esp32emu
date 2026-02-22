// MAX31856 Kiln Temperature Monitor — precision thermocouple readings
#include <MAX31856.h>

Adafruit_MAX31856 tc(5); // CS on pin 5

void setup() {
    Serial.begin(115200);
    Serial.println("MAX31856 Kiln Monitor");

    if (!tc.begin()) {
        Serial.println("ERROR: MAX31856 not found!");
        while (1) delay(10);
    }
    tc.setThermocoupleType(MAX31856_TCTYPE_K);
    tc.setNoiseFilter(MAX31856_NOISE_FILTER_60HZ);
    Serial.println("Type K thermocouple ready");
}

void loop() {
    float tcTemp = tc.readThermocoupleTemperature();
    float cjTemp = tc.readCJTemperature();
    uint8_t fault = tc.readFault();

    Serial.print("🔥 Kiln: ");
    Serial.print(tcTemp, 1);
    Serial.print("°C   Cold junction: ");
    Serial.print(cjTemp, 1);
    Serial.print("°C");
    if (fault) {
        Serial.print("  ⚠️ FAULT: 0x");
        Serial.print(fault, HEX);
    }
    Serial.println();
    delay(1000);
}
