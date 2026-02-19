// BLE Temperature Sensor — advertises a temperature characteristic
#include <BLEDevice.h>

#define SERVICE_UUID        "181A"  // Environmental Sensing
#define TEMP_CHAR_UUID      "2A6E"  // Temperature

BLEServer* pServer = nullptr;
BLECharacteristic* pTempChar = nullptr;
bool deviceConnected = false;
float temperature = 21.5;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer*) override {
        deviceConnected = true;
        Serial.println("Client connected!");
    }
    void onDisconnect(BLEServer*) override {
        deviceConnected = false;
        Serial.println("Client disconnected");
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("BLE Temperature Sensor");

    BLEDevice::init("ESP32-TempSensor");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService* pService = pServer->createService(BLEUUID(static_cast<uint16_t>(0x181A)));
    pTempChar = pService->createCharacteristic(
        BLEUUID(static_cast<uint16_t>(0x2A6E)),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );

    pService->start();

    BLEAdvertising* pAdv = BLEDevice::getAdvertising();
    pAdv->addServiceUUID(BLEUUID(static_cast<uint16_t>(0x181A)));
    pAdv->start();

    Serial.println("BLE advertising started, waiting for connections...");
}

void loop() {
    if (deviceConnected) {
        // Simulate temperature drift
        temperature += (random(-10, 11)) / 10.0;
        if (temperature < -10) temperature = -10;
        if (temperature > 50) temperature = 50;

        // BLE temperature is in 0.01 degree Celsius units
        int16_t tempBLE = (int16_t)(temperature * 100);
        pTempChar->setValue((uint8_t*)&tempBLE, 2);
        pTempChar->notify();

        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");
    }
    delay(2000);
}
