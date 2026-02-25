#include "Arduino.h"
#include "Adafruit_MCP9808.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test 1: Init
    {
        Adafruit_MCP9808 mcp;
        assert(!mcp.isInitialized());
        assert(mcp.begin());
        assert(mcp.isInitialized());
        assert(mcp.getAddress() == 0x18);
    }

    // Test 2: Custom address
    {
        Adafruit_MCP9808 mcp;
        assert(mcp.begin(0x1C));
        assert(mcp.getAddress() == 0x1C);
    }

    // Test 3: Temperature read
    {
        Adafruit_MCP9808 mcp;
        mcp.begin();
        assert(fabs(mcp.readTempC() - 25.0f) < 0.01f); // default
        mcp.setTemperature(37.5f);
        assert(fabs(mcp.readTempC() - 37.5f) < 0.01f);
    }

    // Test 4: Fahrenheit
    {
        Adafruit_MCP9808 mcp;
        mcp.begin();
        mcp.setTemperature(0.0f);
        assert(fabs(mcp.readTempF() - 32.0f) < 0.01f);
        mcp.setTemperature(100.0f);
        assert(fabs(mcp.readTempF() - 212.0f) < 0.01f);
    }

    // Test 5: Resolution
    {
        Adafruit_MCP9808 mcp;
        mcp.begin();
        assert(mcp.getResolution() == 0x03); // max by default
        mcp.setResolution(0x01);
        assert(mcp.getResolution() == 0x01);
        mcp.setResolution(0xFF); // should mask to 0x03
        assert(mcp.getResolution() == 0x03);
    }

    // Test 6: Shutdown/wake
    {
        Adafruit_MCP9808 mcp;
        mcp.begin();
        assert(!mcp.isShutdown());
        mcp.shutdown();
        assert(mcp.isShutdown());
        mcp.wake();
        assert(!mcp.isShutdown());
        mcp.shutdown_wake(true);
        assert(mcp.isShutdown());
        mcp.shutdown_wake(false);
        assert(!mcp.isShutdown());
    }

    // Test 7: Alert thresholds
    {
        Adafruit_MCP9808 mcp;
        mcp.begin();
        mcp.setAlertUpper(40.0f);
        mcp.setAlertLower(5.0f);
        mcp.setAlertCritical(50.0f);
        assert(fabs(mcp.getAlertUpper() - 40.0f) < 0.01f);
        assert(fabs(mcp.getAlertLower() - 5.0f) < 0.01f);
        assert(fabs(mcp.getAlertCritical() - 50.0f) < 0.01f);
    }

    // Test 8: Negative temperature
    {
        Adafruit_MCP9808 mcp;
        mcp.begin();
        mcp.setTemperature(-10.5f);
        assert(fabs(mcp.readTempC() - (-10.5f)) < 0.01f);
        assert(mcp.readTempF() < 32.0f);
    }

    // Test 9: Default alert values
    {
        Adafruit_MCP9808 mcp;
        mcp.begin();
        assert(fabs(mcp.getAlertUpper() - 35.0f) < 0.01f);
        assert(fabs(mcp.getAlertLower() - 10.0f) < 0.01f);
        assert(fabs(mcp.getAlertCritical() - 45.0f) < 0.01f);
    }

    printf("test_adafruit_mcp9808: all assertions passed\n");
    return 0;
}
