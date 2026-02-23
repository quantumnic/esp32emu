#include "MCP9600.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    MCP9600 mcp;
    assert(mcp.begin());

    // Default readings
    assert(std::fabs(mcp.readThermocouple() - 25.0f) < 0.01f);
    assert(std::fabs(mcp.readAmbient() - 22.0f) < 0.01f);
    assert(std::fabs(mcp.readDelta() - 3.0f) < 0.01f);

    // Set temperatures
    mcp.esp32emu_set_hot_junction(250.5f);
    mcp.esp32emu_set_cold_junction(23.1f);
    assert(std::fabs(mcp.readThermocouple() - 250.5f) < 0.01f);
    assert(std::fabs(mcp.readAmbient() - 23.1f) < 0.01f);
    assert(std::fabs(mcp.readDelta() - 227.4f) < 0.1f);

    // Thermocouple type
    mcp.setThermocoupleType(MCP9600_TYPE_J);
    assert(mcp.getThermocoupleType() == MCP9600_TYPE_J);

    // Filter
    mcp.setFilterCoefficient(MCP9600_FILT_MID);
    assert(mcp.getFilterCoefficient() == MCP9600_FILT_MID);

    // ADC resolution
    mcp.setADCResolution(MCP9600_ADCRES_16BIT);
    assert(mcp.getADCResolution() == MCP9600_ADCRES_16BIT);

    // ADC raw
    mcp.esp32emu_set_adc_raw(1.234f);
    assert(std::fabs(mcp.readADC() - 1.234f) < 0.001f);

    // Alert thresholds
    mcp.setAlertTemperature(MCP9600_ALERT1, 150.0f);
    assert(std::fabs(mcp.getAlertTemperature(MCP9600_ALERT1) - 150.0f) < 0.01f);

    // Alert not enabled
    assert(!mcp.alertTriggered(MCP9600_ALERT1));

    // Enable alert — hot junction at 250.5 > 150.0
    mcp.enableAlert(MCP9600_ALERT1, true);
    assert(mcp.alertTriggered(MCP9600_ALERT1));

    // Below threshold
    mcp.setAlertTemperature(MCP9600_ALERT2, 300.0f);
    mcp.enableAlert(MCP9600_ALERT2, true);
    assert(!mcp.alertTriggered(MCP9600_ALERT2));

    // Device ID
    assert(mcp.getDeviceID() == 0x40);
    assert(mcp.getRevision() == 0x01);

    printf("test_mcp9600: all assertions passed\n");
    return 0;
}
