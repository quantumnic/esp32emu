// MH-Z19B CO2 monitor example
// Reads CO2 concentration and temperature periodically

#include <Arduino.h>
#include <MHZ19.h>

MHZ19 mhz19;
HardwareSerial mySerial(2);

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600);
    mhz19.begin(mySerial);

    // Disable auto-calibration for indoor use
    mhz19.autoCalibration(false);
    mhz19.setRange(5000);

    Serial.println("MH-Z19B CO2 Monitor");
    Serial.println("====================");

    // Inject test values for emulation
    mhz19.test_setCO2(650);
    mhz19.test_setTemperature(23.5f);
}

void loop() {
    int co2 = mhz19.getCO2();
    float temp = mhz19.getTemperature();

    Serial.print("CO2: ");
    Serial.print(co2);
    Serial.print(" ppm  |  Temp: ");
    Serial.print(temp);
    Serial.println(" °C");

    if (co2 < 800) {
        Serial.println("  ✅ Air quality: Good");
    } else if (co2 < 1200) {
        Serial.println("  ⚠️  Air quality: Moderate - consider ventilation");
    } else {
        Serial.println("  🔴 Air quality: Poor - open windows!");
    }

    delay(5000);
}
