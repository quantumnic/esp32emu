// LIS3MDL digital compass example
// Reads magnetometer data and calculates heading

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_LIS3MDL.h>
#include <math.h>

Adafruit_LIS3MDL lis;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!lis.begin_I2C()) {
        Serial.println("Failed to find LIS3MDL!");
        while (1) delay(10);
    }
    Serial.println("LIS3MDL Digital Compass");
    Serial.println("========================");

    lis.setRange(LIS3MDL_RANGE_4_GAUSS);
    lis.setDataRate(LIS3MDL_DATARATE_40_HZ);
    lis.setPerformanceMode(LIS3MDL_ULTRAHIGHMODE);
    lis.setOperationMode(LIS3MDL_CONTINUOUSMODE);

    // Inject test magnetic field (pointing roughly North-East)
    lis.test_setMag(0.20f, 0.15f, -0.40f);
}

const char* headingToCardinal(float heading) {
    if (heading < 22.5f || heading >= 337.5f) return "N";
    if (heading < 67.5f)  return "NE";
    if (heading < 112.5f) return "E";
    if (heading < 157.5f) return "SE";
    if (heading < 202.5f) return "S";
    if (heading < 247.5f) return "SW";
    if (heading < 292.5f) return "W";
    return "NW";
}

void loop() {
    lis.read();

    float heading = atan2(lis.y_gauss, lis.x_gauss) * 180.0f / M_PI;
    if (heading < 0) heading += 360.0f;

    float magnitude = sqrt(lis.x_gauss * lis.x_gauss +
                          lis.y_gauss * lis.y_gauss +
                          lis.z_gauss * lis.z_gauss);

    Serial.print("Mag X: "); Serial.print(lis.x_gauss, 3);
    Serial.print(" Y: "); Serial.print(lis.y_gauss, 3);
    Serial.print(" Z: "); Serial.print(lis.z_gauss, 3);
    Serial.print(" Gauss  |  ");
    Serial.print("Heading: "); Serial.print(heading, 1);
    Serial.print("° "); Serial.print(headingToCardinal(heading));
    Serial.print("  |  Magnitude: "); Serial.print(magnitude, 3);
    Serial.println(" Gauss");

    // ASCII compass
    Serial.print("  🧭 ");
    int idx = ((int)(heading + 22.5f) / 45) % 8;
    const char* arrows[] = {"↑ N", "↗ NE", "→ E", "↘ SE", "↓ S", "↙ SW", "← W", "↖ NW"};
    Serial.println(arrows[idx]);

    delay(500);
}
