// MCP9808 precision temperature sensor with alert thresholds
// Demonstrates temperature monitoring with configurable alerts

#include <Arduino.h>
#include <Adafruit_MCP9808.h>

Adafruit_MCP9808 tempsensor;

void setup() {
    Serial.begin(115200);
    Serial.println("MCP9808 Temperature Alert System");

    if (!tempsensor.begin(0x18)) {
        Serial.println("ERROR: MCP9808 not found!");
        while (1) delay(100);
    }

    tempsensor.setResolution(3); // 0.0625°C resolution
    tempsensor.setAlertLower(18.0f);
    tempsensor.setAlertUpper(28.0f);
    tempsensor.setAlertCritical(35.0f);

    Serial.println("Alerts: Low=18°C, High=28°C, Critical=35°C");
}

void loop() {
    tempsensor.wake();
    float c = tempsensor.readTempC();
    float f = tempsensor.readTempF();

    Serial.print("Temp: ");
    Serial.print(c, 2);
    Serial.print("°C / ");
    Serial.print(f, 2);
    Serial.print("°F ");

    if (c >= tempsensor.getAlertCritical())
        Serial.println("🔴 CRITICAL!");
    else if (c >= tempsensor.getAlertUpper())
        Serial.println("🟡 HIGH");
    else if (c <= tempsensor.getAlertLower())
        Serial.println("🔵 LOW");
    else
        Serial.println("🟢 Normal");

    tempsensor.shutdown();
    delay(2000);
}
