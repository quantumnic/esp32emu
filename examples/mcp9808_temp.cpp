// MCP9808 precision temperature sensor example
#include <Arduino.h>
#include "Adafruit_MCP9808.h"

Adafruit_MCP9808 mcp;

void setup() {
    Serial.begin(115200);
    Serial.println("MCP9808 Precision Temperature Sensor");

    if (!mcp.begin()) {
        Serial.println("ERROR: MCP9808 not found!");
        return;
    }
    mcp.setResolution(0x03); // 0.0625°C
    mcp.setAlertUpper(30.0f);
    mcp.setAlertLower(15.0f);

    // Inject test temperature
    mcp.setTemperature(22.5f);
    Serial.println("Sensor ready.");
}

void loop() {
    mcp.wake();
    float c = mcp.readTempC();
    float f = mcp.readTempF();
    Serial.print("Temp: ");
    Serial.print(c, 2);
    Serial.print("°C / ");
    Serial.print(f, 2);
    Serial.println("°F");
    mcp.shutdown();
    delay(2000);
}
