// SHT4x Climate Monitor — reads temperature & humidity from SHT45 sensor
#include <Adafruit_SHT4x.h>

Adafruit_SHT4x sht;

void setup() {
    Serial.begin(115200);
    Serial.println("SHT4x Climate Monitor");

    if (!sht.begin()) {
        Serial.println("ERROR: SHT4x not found!");
        while (1) delay(10);
    }
    sht.setPrecision(SHT4X_HIGH_PRECISION);
    sht.setHeater(SHT4X_NO_HEATER);
    Serial.print("Serial: 0x");
    Serial.println(sht.readSerial(), HEX);
}

void loop() {
    sensors_event_t humidity, temp;
    if (sht.getEvent(&humidity, &temp)) {
        Serial.print("🌡️  ");
        Serial.print(temp.temperature, 1);
        Serial.print("°C   💧 ");
        Serial.print(humidity.relative_humidity, 1);
        Serial.println("%");
    }
    delay(2000);
}
