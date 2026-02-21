// esp32emu example — MLX90614 Contactless IR Thermometer
#include <Arduino.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx;

void setup() {
    Serial.begin(115200);
    Serial.println("=== MLX90614 IR Thermometer ===");

    if (!mlx.begin()) {
        Serial.println("❌ MLX90614 not found!");
        return;
    }
    Serial.println("✅ Sensor ready");
}

void loop() {
    double ambient = mlx.readAmbientTempC();
    double object = mlx.readObjectTempC();

    Serial.printf("Ambient: %.1f°C | Object: %.1f°C\n", ambient, object);

    if (object > 37.5) {
        Serial.println("⚠️  Elevated temperature detected!");
    }

    delay(500);
}
