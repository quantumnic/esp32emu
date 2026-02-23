// MCP9600 Thermocouple Monitor — reads hot/cold junction temps with alerts
#include <MCP9600.h>

MCP9600 mcp;

void setup() {
    Serial.begin(115200);
    Serial.println("MCP9600 Thermocouple Monitor");

    if (!mcp.begin()) {
        Serial.println("ERROR: MCP9600 not found!");
        while (1) delay(10);
    }

    mcp.setThermocoupleType(MCP9600_TYPE_K);
    mcp.setFilterCoefficient(MCP9600_FILT_MID);
    mcp.setADCResolution(MCP9600_ADCRES_18BIT);

    // Alert at 100°C
    mcp.setAlertTemperature(MCP9600_ALERT1, 100.0f);
    mcp.enableAlert(MCP9600_ALERT1, true);

    Serial.print("Device ID: 0x"); Serial.println(mcp.getDeviceID(), HEX);
}

void loop() {
    float hot = mcp.readThermocouple();
    float cold = mcp.readAmbient();
    float delta = mcp.readDelta();

    Serial.print("🔥 Hot: "); Serial.print(hot, 1); Serial.print("°C");
    Serial.print("  ❄️  Cold: "); Serial.print(cold, 1); Serial.print("°C");
    Serial.print("  Δ: "); Serial.print(delta, 1); Serial.println("°C");

    if (mcp.alertTriggered(MCP9600_ALERT1)) {
        Serial.println("⚠️  ALERT: Temperature exceeds 100°C!");
    }
    delay(500);
}
