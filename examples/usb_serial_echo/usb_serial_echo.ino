// USB Serial/JTAG echo — reads from USB serial and echoes back
#include <Arduino.h>
#include "driver/usb_serial_jtag.h"

void setup() {
    Serial.begin(115200);

    usb_serial_jtag_driver_config_t config = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    config.rx_buffer_size = 512;
    config.tx_buffer_size = 512;
    usb_serial_jtag_driver_install(&config);

    Serial.println("USB Serial/JTAG echo ready");
}

void loop() {
    char buf[64];
    int n = usb_serial_jtag_read_bytes(buf, sizeof(buf) - 1, 100);
    if (n > 0) {
        buf[n] = '\0';
        Serial.print("Received: ");
        Serial.println(buf);
        usb_serial_jtag_write_bytes(buf, n, 100);
    }
    delay(10);
}
