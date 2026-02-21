// esp32emu test — Adafruit MLX90614 IR Thermometer
#include <cassert>
#include <cstdio>
#include <cmath>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "Adafruit_MLX90614.h"

int main() {
    // Test init
    {
        Adafruit_MLX90614 mlx;
        assert(!mlx.test_isInitialized());
        assert(mlx.begin());
        assert(mlx.test_isInitialized());
    }

    // Test default temperatures
    {
        Adafruit_MLX90614 mlx;
        mlx.begin();
        assert(fabs(mlx.readObjectTempC() - 36.5) < 0.01);
        assert(fabs(mlx.readAmbientTempC() - 22.0) < 0.01);
    }

    // Test Fahrenheit conversion
    {
        Adafruit_MLX90614 mlx;
        mlx.begin();
        double objF = mlx.readObjectTempF();
        assert(fabs(objF - (36.5 * 9.0 / 5.0 + 32.0)) < 0.01);
    }

    // Test custom temperatures
    {
        Adafruit_MLX90614 mlx;
        mlx.begin();
        mlx.test_setObjectTemp(100.0);
        mlx.test_setAmbientTemp(30.0);
        assert(fabs(mlx.readObjectTempC() - 100.0) < 0.01);
        assert(fabs(mlx.readAmbientTempC() - 30.0) < 0.01);
    }

    // Test emissivity
    {
        Adafruit_MLX90614 mlx;
        mlx.begin();
        assert(mlx.readEmissivity() == 0xFFFF);
        mlx.writeEmissivity(0x8000);
        assert(mlx.readEmissivity() == 0x8000);
    }

    // Test not initialized returns 0
    {
        Adafruit_MLX90614 mlx;
        assert(fabs(mlx.readObjectTempC()) < 0.01);
        assert(fabs(mlx.readAmbientTempC()) < 0.01);
    }

    printf("test_mlx90614: all assertions passed\n");
    return 0;
}
