#include "BluetoothSerial.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Test begin/end
    {
        BluetoothSerial bt;
        assert(bt.begin("TestESP32"));
        assert(bt.isReady());
        bt.end();
        assert(!bt.isReady());
    }

    // Test string name begin
    {
        BluetoothSerial bt;
        std::string name = "MyDevice";
        assert(bt.begin(name));
        assert(bt.isReady());
    }

    // Test connect/disconnect
    {
        BluetoothSerial bt;
        bt.begin("ESP32");
        assert(!bt.hasClient());
        assert(bt.connect("RemoteDevice"));
        assert(bt.hasClient());
        assert(bt.disconnect());
        assert(!bt.hasClient());
    }

    // Test simulate connect/disconnect
    {
        BluetoothSerial bt;
        bt.begin("ESP32");
        bt.simulateConnect();
        assert(bt.hasClient());
        bt.simulateDisconnect();
        assert(!bt.hasClient());
    }

    // Test write and getTxData
    {
        BluetoothSerial bt;
        bt.begin("ESP32");
        bt.write('H');
        bt.write('i');
        assert(bt.getTxData() == "Hi");
        bt.clearTx();
        assert(bt.getTxData().empty());
    }

    // Test print/println
    {
        BluetoothSerial bt;
        bt.begin("ESP32");
        bt.print("Hello");
        bt.println(" World");
        assert(bt.getTxData() == "Hello World\r\n");
    }

    // Test print int
    {
        BluetoothSerial bt;
        bt.begin("ESP32");
        bt.print(42);
        assert(bt.getTxData() == "42");
        bt.clearTx();
        bt.println(99);
        assert(bt.getTxData() == "99\r\n");
    }

    // Test write buffer
    {
        BluetoothSerial bt;
        bt.begin("ESP32");
        uint8_t data[] = {0x01, 0x02, 0x03};
        size_t written = bt.write(data, 3);
        assert(written == 3);
        assert(bt.getTxData().size() == 3);
    }

    // Test inject and read
    {
        BluetoothSerial bt;
        bt.begin("ESP32");
        assert(bt.available() == 0);
        assert(bt.read() == -1);

        bt.injectRx("Hello");
        assert(bt.available() == 5);
        assert(bt.read() == 'H');
        assert(bt.available() == 4);
    }

    // Test readString
    {
        BluetoothSerial bt;
        bt.begin("ESP32");
        bt.injectRx("Test Data");
        std::string s = bt.readString();
        assert(s == "Test Data");
        assert(bt.available() == 0);
    }

    // Test readStringUntil
    {
        BluetoothSerial bt;
        bt.begin("ESP32");
        bt.injectRx("key=value\n");
        std::string key = bt.readStringUntil('=');
        assert(key == "key");
        std::string val = bt.readStringUntil('\n');
        assert(val == "value");
    }

    // Test readBytes
    {
        BluetoothSerial bt;
        bt.begin("ESP32");
        uint8_t raw[] = {0xAA, 0xBB, 0xCC, 0xDD};
        bt.injectRx(raw, 4);
        uint8_t out[4];
        size_t n = bt.readBytes(out, 4);
        assert(n == 4);
        assert(out[0] == 0xAA);
        assert(out[3] == 0xDD);
    }

    // Test end clears buffer
    {
        BluetoothSerial bt;
        bt.begin("ESP32");
        bt.injectRx("leftover");
        bt.end();
        assert(bt.available() == 0);
    }

    printf("test_bluetoothserial: all assertions passed\n");
    return 0;
}
