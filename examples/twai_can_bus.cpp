// ESP32 TWAI (CAN bus) communication example
#include "Arduino.h"
#include "driver_twai.h"

void setup() {
    Serial.begin(115200);

    twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT(21, 22, TWAI_MODE_NORMAL);
    twai_timing_config_t t = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    twai_driver_install(&g, &t, &f);
    twai_start();
    Serial.println("TWAI started at 500kbps");

    // Send a message
    twai_message_t msg = {};
    msg.identifier = 0x100;
    msg.data_length_code = 4;
    msg.data[0] = 0x01;
    msg.data[1] = 0x02;
    msg.data[2] = 0x03;
    msg.data[3] = 0x04;
    twai_transmit(&msg, pdMS_TO_TICKS(100));
    printf("Sent CAN message ID=0x%03X\n", msg.identifier);
}

void loop() {
    twai_message_t rx;
    if (twai_receive(&rx, pdMS_TO_TICKS(100)) == ESP_OK) {
        printf("Received ID=0x%03X DLC=%d Data:", rx.identifier, rx.data_length_code);
        for (int i = 0; i < rx.data_length_code; i++)
            printf(" %02X", rx.data[i]);
        printf("\n");
    }
    delay(100);
}
