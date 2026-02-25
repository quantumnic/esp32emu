#include "Arduino.h"
#include "Adafruit_seesaw.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test 1: Init
    {
        Adafruit_seesaw ss;
        assert(ss.begin(0x49));
        assert(ss.getHWID() == SEESAW_HW_ID_CODE);
    }

    // Test 2: Temperature
    {
        Adafruit_seesaw ss;
        ss.begin();
        ss.esp32emu_set_temperature(28.5f);
        assert(fabs(ss.getTemp() - 28.5f) < 0.01f);
    }

    // Test 3: GPIO
    {
        Adafruit_seesaw ss;
        ss.begin();
        ss.pinMode(5, 1); // OUTPUT
        ss.digitalWrite(5, 1);
        assert(ss.digitalRead(5) == true);
        ss.digitalWrite(5, 0);
        assert(ss.digitalRead(5) == false);
    }

    // Test 4: GPIO via test helper
    {
        Adafruit_seesaw ss;
        ss.begin();
        ss.esp32emu_set_gpio(10, true);
        assert(ss.digitalRead(10) == true);
    }

    // Test 5: ADC
    {
        Adafruit_seesaw ss;
        ss.begin();
        ss.esp32emu_set_adc(0, 512);
        ss.esp32emu_set_adc(3, 1023);
        assert(ss.analogRead(0) == 512);
        assert(ss.analogRead(3) == 1023);
        assert(ss.analogRead(8) == 0); // out of range
    }

    // Test 6: Encoder
    {
        Adafruit_seesaw ss;
        ss.begin();
        ss.esp32emu_set_encoder_position(42);
        assert(ss.getEncoderPosition() == 42);

        ss.setEncoderPosition(0);
        assert(ss.getEncoderPosition() == 0);

        ss.esp32emu_set_encoder_delta(5);
        assert(ss.getEncoderDelta() == 5);
        assert(ss.getEncoderDelta() == 0); // delta resets after read
    }

    // Test 7: NeoPixel
    {
        Adafruit_seesaw ss;
        ss.begin();
        ss.setBrightness(128);
        assert(ss.esp32emu_get_brightness() == 128);

        ss.setPixelColor(0, 0xFF0000);
        ss.setPixelColor(1, 0x00FF00);
        ss.show();
        assert(ss.esp32emu_neopixel_shown());
        assert(ss.esp32emu_get_pixel_color(0) == 0xFF0000);
        assert(ss.esp32emu_get_pixel_color(1) == 0x00FF00);
    }

    // Test 8: Keypad
    {
        Adafruit_seesaw ss;
        ss.begin();
        ss.enableKeypadInterrupt();

        ss.esp32emu_add_keypad_event(3, SEESAW_KEYPAD_EDGE_RISING);
        ss.esp32emu_add_keypad_event(7, SEESAW_KEYPAD_EDGE_FALLING);
        assert(ss.getKeypadCount() == 2);

        keyEventRaw events[4];
        assert(ss.readKeypad(events, 4));
        assert(events[0].bit.NUM == 3);
        assert(events[0].bit.EDGE == SEESAW_KEYPAD_EDGE_RISING);
        assert(events[1].bit.NUM == 7);
        assert(ss.getKeypadCount() == 0); // cleared after read
    }

    // Test 9: Touch (soil sensor)
    {
        Adafruit_seesaw ss;
        ss.begin();
        ss.esp32emu_set_touch(0, 800);
        assert(ss.touchRead(0) == 800);
        assert(ss.touchRead(4) == 0); // out of range
    }

    // Test 10: Version & options
    {
        Adafruit_seesaw ss;
        ss.begin();
        ss.esp32emu_set_version(0x20490001);
        assert(ss.getVersion() == 0x20490001);
        ss.esp32emu_set_options(0xABCD1234);
        assert(ss.getOptions() == 0xABCD1234);
    }

    // Test 11: RotaryEncoder subclass
    {
        Adafruit_seesaw_RotaryEncoder enc;
        assert(enc.begin(0x36));
        enc.esp32emu_set_encoder_position(100);
        assert(enc.getEncoderPosition() == 100);
    }

    // Test 12: Generic read/write
    {
        Adafruit_seesaw ss;
        ss.begin();
        uint8_t buf[4] = {};
        assert(ss.write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, buf, 4));
        assert(ss.read(SEESAW_ADC_BASE, 0, buf, 4));
    }

    // Test 13: Software reset
    {
        Adafruit_seesaw ss;
        ss.begin();
        ss.SWReset(); // should not crash
    }

    printf("test_adafruit_seesaw: all assertions passed\n");
    return 0;
}
