// BH1750 ambient light sensor example
#include "Arduino.h"
#include "BH1750.h"

BH1750 lightMeter(0x23);

void setup() {
    Serial.begin(115200);
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
    lightMeter.setLux(350.0f);  // Simulate indoor lighting
    Serial.println("BH1750 ready");
}

void loop() {
    if (lightMeter.measurementReady()) {
        float lux = lightMeter.readLightLevel();
        printf("Light: %.1f lx\n", lux);

        if (lux < 100) printf("  -> Dark (night/closet)\n");
        else if (lux < 500) printf("  -> Indoor lighting\n");
        else if (lux < 10000) printf("  -> Daylight indoor\n");
        else printf("  -> Direct sunlight\n");
    }
    delay(1000);
}
