// esp32emu example — ezButton debounce library
#include <Arduino.h>
#include <ezButton.h>

ezButton button(4);  // GPIO4

void setup() {
    Serial.begin(115200);
    button.setDebounceTime(50);
    button.setCountMode(ezButton::COUNT_FALLING);
    Serial.println("ezButton demo — press the button!");
}

void loop() {
    button.loop();

    if (button.isPressed()) {
        Serial.print("Button PRESSED! Count: ");
        Serial.println(button.getCount());
    }

    if (button.isReleased()) {
        Serial.println("Button released");
    }

    delay(10);
}
