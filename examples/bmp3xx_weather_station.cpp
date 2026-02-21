// esp32emu example: BMP388 Weather Station
// Reads temperature, pressure and calculates altitude.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP3XX.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!bmp.begin_I2C()) {
        Serial.println("BMP388 not found!");
        while (1) delay(10);
    }

    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);

    // Test: mountain location
    bmp.setReadings(18.5f, 95000.0f);

    Serial.println("BMP388 Weather Station");
}

void loop() {
    if (bmp.performReading()) {
        Serial.print("Temperature: "); Serial.print(bmp.temperature, 1); Serial.println(" °C");
        Serial.print("Pressure: "); Serial.print(bmp.pressure / 100.0f, 2); Serial.println(" hPa");
        Serial.print("Altitude: "); Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA * 100), 1); Serial.println(" m");
        Serial.println("---");
    }
    delay(2000);
}
