/*
 * IR Remote Control — Send and receive infrared codes.
 * Demonstrates NEC protocol send/receive with IRremote library.
 */

#include <IRremote.h>

IRrecv receiver(11);
IRsend sender(3);

void setup() {
    Serial.begin(115200);
    receiver.enableIRIn();
    sender.begin();
    Serial.println("IR Remote Control demo");

    // Simulate receiving a TV power button press
    receiver.injectCode(NEC, 0x20DF10EF, 32);  // LG TV Power
    receiver.injectCode(NEC, 0x20DF40BF, 32);  // LG TV Vol+
}

void loop() {
    decode_results results;

    if (receiver.decode(&results)) {
        Serial.print("Received IR code: 0x");
        char buf[12];
        snprintf(buf, sizeof(buf), "%08X", results.value);
        Serial.println(buf);

        // Echo back the code
        sender.sendNEC(results.value);
        Serial.println("Echoed code back via IR LED");

        receiver.resume();
    } else {
        Serial.println("No more IR codes. Sending test pattern...");
        sender.sendNEC(0xFF00FF00);
        sender.sendSony(0xA90, 12);
        Serial.println("Done!");
        while (true) delay(1000);
    }

    delay(500);
}
