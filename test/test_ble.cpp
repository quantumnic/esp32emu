// test_ble: BLE device/server/service/characteristic mock tests
#include "BLEDevice.h"
#include <cassert>
#include <cstdio>
#include <cstring>

// Test callback
class TestCallbacks : public BLECharacteristicCallbacks {
public:
    int write_count = 0;
    std::string last_value;
    void onWrite(BLECharacteristic* c) override {
        write_count++;
        last_value = c->getValue();
    }
};

class TestServerCallbacks : public BLEServerCallbacks {
public:
    int connects = 0;
    int disconnects = 0;
    void onConnect(BLEServer*) override { connects++; }
    void onDisconnect(BLEServer*) override { disconnects++; }
};

int main() {
    ble_test::reset();

    // Init
    BLEDevice::init("TestDevice");
    assert(BLEDevice::isInitialized());
    assert(BLEDevice::getDeviceName() == "TestDevice");

    // Create server
    BLEServer* server = BLEDevice::createServer();
    assert(server != nullptr);

    TestServerCallbacks serverCb;
    server->setCallbacks(&serverCb);

    // Create service
    BLEUUID serviceUUID("12345678-1234-1234-1234-123456789abc");
    BLEService* service = server->createService(serviceUUID);
    assert(service != nullptr);
    assert(server->getServiceCount() == 1);

    // Create characteristics
    BLEUUID charUUID("abcdef01-1234-1234-1234-123456789abc");
    BLECharacteristic* charac = service->createCharacteristic(
        charUUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    assert(charac != nullptr);
    assert(service->getCharacteristicCount() == 1);

    // Set/get value
    charac->setValue("Hello BLE");
    assert(charac->getValue() == "Hello BLE");
    assert(charac->getLength() == 9);

    // Notify
    charac->notify();
    assert(charac->getNotifyCount() == 1);
    charac->notify();
    assert(charac->getNotifyCount() == 2);

    // Callbacks
    TestCallbacks cb;
    charac->setCallbacks(&cb);
    charac->simulateWrite("test_data");
    assert(cb.write_count == 1);
    assert(cb.last_value == "test_data");
    assert(charac->getValue() == "test_data");

    // Start service
    service->start();
    assert(service->isStarted());

    // Advertising
    BLEAdvertising* adv = server->getAdvertising();
    adv->addServiceUUID(serviceUUID);
    assert(adv->getServiceUUIDCount() == 1);
    adv->start();
    assert(adv->isAdvertising());
    adv->stop();
    assert(!adv->isAdvertising());

    // Server connect/disconnect
    server->simulateConnect();
    assert(server->getConnectedCount() == 1);
    assert(serverCb.connects == 1);
    server->simulateConnect();
    assert(server->getConnectedCount() == 2);
    server->simulateDisconnect();
    assert(server->getConnectedCount() == 1);
    assert(serverCb.disconnects == 1);

    // UUID from uint16
    BLEUUID uuid16(static_cast<uint16_t>(0x180A));
    assert(uuid16.toString() == "0000180a-0000-1000-8000-00805f9b34fb");

    // Lookup service/characteristic
    assert(server->getServiceByUUID(serviceUUID) == service);
    assert(service->getCharacteristic(charUUID) == charac);

    // MTU
    BLEDevice::setMTU(256);
    assert(BLEDevice::getMTU() == 256);

    // Indicate
    charac->indicate();
    assert(charac->getIndicateCount() == 1);

    // Descriptor
    BLEDescriptor desc(BLEUUID(static_cast<uint16_t>(0x2902)));
    desc.setValue("desc_val");
    assert(desc.getValue() == "desc_val");
    charac->addDescriptor(&desc);

    // Cleanup
    ble_test::reset();
    assert(!BLEDevice::isInitialized());

    printf("test_ble: all assertions passed\n");
    return 0;
}
