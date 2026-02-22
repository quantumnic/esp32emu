// esp32emu example — AMG88xx Thermal Camera
// Read 8x8 thermal pixel array and display as heatmap.
#include <Adafruit_AMG88xx.h>

Adafruit_AMG88xx amg;
float pixels[AMG88xx_PIXEL_ARRAY_SIZE];

void setup() {
    Serial.begin(115200);
    Serial.println("AMG88xx Thermal Camera Demo");

    if (!amg.begin()) {
        Serial.println("AMG88xx not found!");
        while (1) delay(100);
    }

    amg.setFrameRate(AMG88xx_FPS_10);
    Serial.printf("Thermistor: %.1f°C\n", amg.readThermistor());

    // Inject test data for demo
    for (int i = 0; i < 64; i++) {
        amg._injectPixel(i, 20.0f + (float)i * 0.3f);
    }
}

void loop() {
    amg.readPixels(pixels);

    Serial.println("\n--- Thermal Image (°C) ---");
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            float t = pixels[row * 8 + col];
            if (t > 30) Serial.print("🟥");
            else if (t > 25) Serial.print("🟧");
            else if (t > 22) Serial.print("🟨");
            else Serial.print("🟦");
        }
        Serial.println();
    }

    // Find hotspot
    float maxTemp = -999;
    int maxIdx = 0;
    for (int i = 0; i < 64; i++) {
        if (pixels[i] > maxTemp) { maxTemp = pixels[i]; maxIdx = i; }
    }
    Serial.printf("Hotspot: [%d,%d] = %.1f°C\n", maxIdx % 8, maxIdx / 8, maxTemp);

    delay(1000);
}
