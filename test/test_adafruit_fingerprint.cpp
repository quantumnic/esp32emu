#include "Arduino.h"
#include "Adafruit_Fingerprint.h"
#include <cassert>
#include <cstdio>

int main() {
    int dummy_serial = 0;

    // Test 1: Init
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        assert(finger.begin(57600));
        assert(finger.verifyPassword());
    }

    // Test 2: No finger present
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        assert(finger.getImage() == FINGERPRINT_NOFINGER);
    }

    // Test 3: Finger present + image capture
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        finger.esp32emu_set_finger_present(true);
        assert(finger.getImage() == FINGERPRINT_OK);
        assert(finger.image2Tz(1) == FINGERPRINT_OK);
    }

    // Test 4: Search - not found
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        finger.esp32emu_clear_match();
        assert(finger.fingerSearch() == FINGERPRINT_NOTFOUND);
    }

    // Test 5: Search - found
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        finger.esp32emu_set_match(42, 200);
        assert(finger.fingerSearch() == FINGERPRINT_OK);
        assert(finger.fingerID == 42);
        assert(finger.confidence == 200);
    }

    // Test 6: Enroll (store model)
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        assert(finger.createModel() == FINGERPRINT_OK);
        assert(finger.storeModel(10) == FINGERPRINT_OK);
        assert(finger.esp32emu_is_stored(10));
        assert(finger.esp32emu_get_template_count() == 1);
    }

    // Test 7: Template count
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        finger.storeModel(0);
        finger.storeModel(5);
        finger.storeModel(10);
        assert(finger.getTemplateCount() == FINGERPRINT_OK);
        assert(finger.templateCount == 3);
    }

    // Test 8: Delete model
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        finger.storeModel(7);
        assert(finger.esp32emu_is_stored(7));
        assert(finger.deleteModel(7) == FINGERPRINT_OK);
        assert(!finger.esp32emu_is_stored(7));
    }

    // Test 9: Empty database
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        finger.storeModel(1);
        finger.storeModel(2);
        finger.storeModel(3);
        assert(finger.emptyDatabase() == FINGERPRINT_OK);
        assert(finger.esp32emu_get_template_count() == 0);
        assert(!finger.esp32emu_is_stored(1));
    }

    // Test 10: Load model
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        assert(finger.loadModel(50) == FINGERPRINT_BADLOCATION); // not stored
        finger.storeModel(50);
        assert(finger.loadModel(50) == FINGERPRINT_OK);
    }

    // Test 11: Bad location
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        assert(finger.storeModel(250) == FINGERPRINT_BADLOCATION);
    }

    // Test 12: Parameters
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        assert(finger.getParameters() == FINGERPRINT_OK);
        assert(finger.setSecurityLevel(5) == FINGERPRINT_OK);
        assert(finger.security_level == 5);
    }

    // Test 13: LED control
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        assert(finger.LEDcontrol(1, 100, 2, 3) == FINGERPRINT_OK);
    }

    // Test 14: Fast search
    {
        Adafruit_Fingerprint finger(&dummy_serial);
        finger.begin();
        finger.esp32emu_set_match(5, 150);
        assert(finger.fingerFastSearch() == FINGERPRINT_OK);
        assert(finger.fingerID == 5);
    }

    printf("test_adafruit_fingerprint: all assertions passed\n");
    return 0;
}
