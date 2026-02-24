#include <Arduino.h>
#include <Adafruit_DRV2605.h>

Adafruit_DRV2605 drv;

void setup() {
    Serial.begin(115200);
    Serial.println("DRV2605 Haptic Driver Demo");

    if (!drv.begin()) {
        Serial.println("Failed to find DRV2605!");
        while (1) delay(10);
    }

    drv.selectLibrary(1);
    drv.setMode(Adafruit_DRV2605::DRV2605_MODE_INTTRIG);
    Serial.println("DRV2605 initialized (ERM mode)");
}

void loop() {
    // Play a sequence of haptic effects
    static uint8_t effect = 1;

    drv.setWaveform(0, effect);
    drv.setWaveform(1, 0); // end
    drv.go();

    Serial.print("Playing effect #");
    Serial.println(effect);

    effect++;
    if (effect > 117) effect = 1;

    delay(500);
}
