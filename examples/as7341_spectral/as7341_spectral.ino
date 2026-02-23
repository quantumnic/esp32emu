// AS7341 Spectral Color Sensor — reads 10 spectral channels + flicker detection
#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;

void setup() {
    Serial.begin(115200);
    Serial.println("AS7341 Spectral Sensor");

    if (!as7341.begin()) {
        Serial.println("ERROR: AS7341 not found!");
        while (1) delay(10);
    }

    as7341.setATIME(100);
    as7341.setASTEP(999);
    as7341.setGain(AS7341_GAIN_256X);
    as7341.enableLED(true);
    as7341.setLEDCurrent(AS7341_LED_50MA);

    Serial.print("Integration time: ");
    Serial.print(as7341.getTINT(), 1);
    Serial.println(" ms");
}

void loop() {
    if (as7341.readAllChannels()) {
        Serial.println("── Spectral Channels ──");
        Serial.print("🟣 415nm F1: "); Serial.println(as7341.getChannel(AS7341_CHANNEL_415nm_F1));
        Serial.print("🔵 445nm F2: "); Serial.println(as7341.getChannel(AS7341_CHANNEL_445nm_F2));
        Serial.print("🔵 480nm F3: "); Serial.println(as7341.getChannel(AS7341_CHANNEL_480nm_F3));
        Serial.print("🟢 515nm F4: "); Serial.println(as7341.getChannel(AS7341_CHANNEL_515nm_F4));
        Serial.print("🟢 555nm F5: "); Serial.println(as7341.getChannel(AS7341_CHANNEL_555nm_F5));
        Serial.print("🟡 590nm F6: "); Serial.println(as7341.getChannel(AS7341_CHANNEL_590nm_F6));
        Serial.print("🟠 630nm F7: "); Serial.println(as7341.getChannel(AS7341_CHANNEL_630nm_F7));
        Serial.print("🔴 680nm F8: "); Serial.println(as7341.getChannel(AS7341_CHANNEL_680nm_F8));
        Serial.print("⬜ Clear:    "); Serial.println(as7341.getChannel(AS7341_CHANNEL_CLEAR));
        Serial.print("🌑 NIR:      "); Serial.println(as7341.getChannel(AS7341_CHANNEL_NIR));

        uint16_t flicker = as7341.detectFlickerHz();
        if (flicker > 0) {
            Serial.print("💡 Flicker: "); Serial.print(flicker); Serial.println(" Hz");
        }
        Serial.println();
    }
    delay(1000);
}
