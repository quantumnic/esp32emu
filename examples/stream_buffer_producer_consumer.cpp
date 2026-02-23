#include <Arduino.h>
#include "freertos/stream_buffer.h"

StreamBufferHandle_t stream;

void setup() {
    Serial.begin(115200);
    stream = xStreamBufferCreate(128, 1);
    Serial.println("Stream Buffer Producer/Consumer Demo");

    // Producer: write sensor data
    const char* data[] = {"temp=22.5", "hum=45.0", "pres=1013"};
    for (int i = 0; i < 3; i++) {
        size_t sent = xStreamBufferSend(stream, data[i], strlen(data[i]) + 1, 100);
        Serial.print("Sent ");
        Serial.print(sent);
        Serial.print(" bytes: ");
        Serial.println(data[i]);
    }

    Serial.print("Bytes in buffer: ");
    Serial.println(xStreamBufferBytesAvailable(stream));

    // Consumer: read all
    char buf[64];
    while (xStreamBufferBytesAvailable(stream) > 0) {
        size_t rx = xStreamBufferReceive(stream, buf, sizeof(buf), 0);
        if (rx > 0) {
            Serial.print("Received: ");
            Serial.println(buf);
        }
    }

    Serial.println("Buffer empty: ");
    Serial.println(xStreamBufferIsEmpty(stream) ? "yes" : "no");

    vStreamBufferDelete(stream);
    Serial.println("Done!");
}

void loop() {
    delay(1000);
}
