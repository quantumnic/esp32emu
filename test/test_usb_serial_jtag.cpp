// test_usb_serial_jtag: USB Serial/JTAG driver mock tests
#include "driver/usb_serial_jtag.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    usb_serial_jtag_test::reset();

    // Install driver
    usb_serial_jtag_driver_config_t config = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    assert(usb_serial_jtag_driver_install(&config) == ESP_OK);
    assert(usb_serial_jtag_test::installed());

    // Write
    const char* msg = "Hello USB";
    int written = usb_serial_jtag_write_bytes(msg, strlen(msg), 100);
    assert(written == (int)strlen(msg));
    assert(usb_serial_jtag_test::tx_buf().size() == strlen(msg));
    assert(memcmp(usb_serial_jtag_test::tx_buf().data(), msg, strlen(msg)) == 0);

    // Read with injected data
    usb_serial_jtag_test::injectRx("Test", 4);
    char buf[16] = {};
    int n = usb_serial_jtag_read_bytes(buf, sizeof(buf), 100);
    assert(n == 4);
    assert(memcmp(buf, "Test", 4) == 0);

    // Read empty
    n = usb_serial_jtag_read_bytes(buf, sizeof(buf), 100);
    assert(n == 0);

    // Partial read
    usb_serial_jtag_test::injectRx("ABCDEF", 6);
    n = usb_serial_jtag_read_bytes(buf, 3, 100);
    assert(n == 3);
    assert(memcmp(buf, "ABC", 3) == 0);
    n = usb_serial_jtag_read_bytes(buf, 10, 100);
    assert(n == 3);
    assert(memcmp(buf, "DEF", 3) == 0);

    // Uninstall
    assert(usb_serial_jtag_driver_uninstall() == ESP_OK);
    assert(!usb_serial_jtag_test::installed());

    printf("test_usb_serial_jtag: all assertions passed\n");
    return 0;
}
