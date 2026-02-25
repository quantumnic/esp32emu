// esp32emu example — TMP117 high-accuracy temperature sensor
#include <Arduino.h>
#include <Adafruit_TMP117.h>

Adafruit_TMP117 tmp117;

void setup() {
    Serial.begin(115200);
    if (!tmp117.begin()) {
        Serial.println("TMP117 not found!");
        while (1) delay(10);
    }
    Serial.println("TMP117 initialized");
    tmp117.setRate(TMP117_RATE_4_HZ);
    tmp117.setAveraging(TMP117_AVERAGE_8X);

    // Set alert thresholds
    tmp117.setHighThreshold(35.0);
    tmp117.setLowThreshold(15.0);

    Serial.print("Device ID: 0x");
    Serial.println(tmp117.getDeviceID(), HEX);
}

void loop() {
    if (tmp117.dataReady()) {
        float temp = tmp117.readTempC();
        Serial.print("Temperature: ");
        Serial.print(temp, 4);
        Serial.println(" °C");

        tmp117_alert_t alert = tmp117.getAlert();
        if (alert == TMP117_ALERT_HIGH) Serial.println("⚠️ HIGH temperature alert!");
        if (alert == TMP117_ALERT_LOW)  Serial.println("⚠️ LOW temperature alert!");
    }
    delay(250);
}
