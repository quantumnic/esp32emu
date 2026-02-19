// BLEDevice.h — ESP32 BLE mock for esp32emu
// Mocks: BLEDevice, BLEServer, BLEService, BLECharacteristic, BLEAdvertising
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <map>
#include <string>
#include <vector>

// Forward declarations
class BLEServer;
class BLEService;
class BLECharacteristic;
class BLEAdvertising;

// ── UUID ────────────────────────────────────────────────────────────
class BLEUUID {
public:
    BLEUUID() = default;
    BLEUUID(const char* uuid) : uuid_(uuid) {}
    BLEUUID(uint16_t uuid16) {
        char buf[40];
        snprintf(buf, sizeof(buf), "0000%04x-0000-1000-8000-00805f9b34fb", uuid16);
        uuid_ = buf;
    }
    std::string toString() const { return uuid_; }
    bool operator==(const BLEUUID& o) const { return uuid_ == o.uuid_; }
    bool operator<(const BLEUUID& o) const { return uuid_ < o.uuid_; }
private:
    std::string uuid_;
};

// ── Descriptor ──────────────────────────────────────────────────────
class BLEDescriptor {
public:
    BLEDescriptor(BLEUUID uuid) : uuid_(uuid) {}
    void setValue(const std::string& v) { value_ = v; }
    std::string getValue() const { return value_; }
    BLEUUID getUUID() const { return uuid_; }
private:
    BLEUUID uuid_;
    std::string value_;
};

// ── Characteristic Callbacks ────────────────────────────────────────
class BLECharacteristicCallbacks {
public:
    virtual ~BLECharacteristicCallbacks() = default;
    virtual void onRead(BLECharacteristic* /*c*/) {}
    virtual void onWrite(BLECharacteristic* /*c*/) {}
};

// ── Characteristic ──────────────────────────────────────────────────
class BLECharacteristic {
public:
    static constexpr uint32_t PROPERTY_READ    = 0x02;
    static constexpr uint32_t PROPERTY_WRITE   = 0x08;
    static constexpr uint32_t PROPERTY_NOTIFY  = 0x10;
    static constexpr uint32_t PROPERTY_INDICATE = 0x20;

    BLECharacteristic(BLEUUID uuid, uint32_t properties)
        : uuid_(uuid), properties_(properties) {}

    void setValue(const std::string& v) { value_ = std::vector<uint8_t>(v.begin(), v.end()); }
    void setValue(uint8_t* data, size_t len) { value_.assign(data, data + len); }
    void setValue(uint32_t v) {
        value_.resize(4);
        memcpy(value_.data(), &v, 4);
    }
    std::string getValue() const { return std::string(value_.begin(), value_.end()); }
    uint8_t* getData() { return value_.data(); }
    size_t getLength() const { return value_.size(); }

    void notify() {
        notify_count_++;
        fprintf(stderr, "[esp32emu] BLE notify: %s (%zu bytes)\n",
                uuid_.toString().c_str(), value_.size());
    }
    void indicate() {
        indicate_count_++;
        fprintf(stderr, "[esp32emu] BLE indicate: %s (%zu bytes)\n",
                uuid_.toString().c_str(), value_.size());
    }

    void setCallbacks(BLECharacteristicCallbacks* cb) { callbacks_ = cb; }
    BLECharacteristicCallbacks* getCallbacks() const { return callbacks_; }

    void addDescriptor(BLEDescriptor* d) { descriptors_.push_back(d); }
    BLEUUID getUUID() const { return uuid_; }
    uint32_t getProperties() const { return properties_; }

    // Test helpers
    int getNotifyCount() const { return notify_count_; }
    int getIndicateCount() const { return indicate_count_; }
    void simulateWrite(const std::string& data) {
        value_.assign(data.begin(), data.end());
        if (callbacks_) callbacks_->onWrite(this);
    }

private:
    BLEUUID uuid_;
    uint32_t properties_;
    std::vector<uint8_t> value_;
    BLECharacteristicCallbacks* callbacks_ = nullptr;
    std::vector<BLEDescriptor*> descriptors_;
    int notify_count_ = 0;
    int indicate_count_ = 0;
};

// ── Service ─────────────────────────────────────────────────────────
class BLEService {
public:
    BLEService(BLEUUID uuid) : uuid_(uuid) {}

    BLECharacteristic* createCharacteristic(BLEUUID uuid, uint32_t properties) {
        auto* c = new BLECharacteristic(uuid, properties);
        chars_[uuid.toString()] = c;
        return c;
    }

    BLECharacteristic* getCharacteristic(BLEUUID uuid) {
        auto it = chars_.find(uuid.toString());
        return it != chars_.end() ? it->second : nullptr;
    }

    void start() {
        started_ = true;
        fprintf(stderr, "[esp32emu] BLE service started: %s\n", uuid_.toString().c_str());
    }

    bool isStarted() const { return started_; }
    BLEUUID getUUID() const { return uuid_; }
    size_t getCharacteristicCount() const { return chars_.size(); }

    ~BLEService() {
        for (auto& p : chars_) delete p.second;
    }

private:
    BLEUUID uuid_;
    std::map<std::string, BLECharacteristic*> chars_;
    bool started_ = false;
};

// ── Server Callbacks ────────────────────────────────────────────────
class BLEServerCallbacks {
public:
    virtual ~BLEServerCallbacks() = default;
    virtual void onConnect(BLEServer* /*s*/) {}
    virtual void onDisconnect(BLEServer* /*s*/) {}
};

// ── Server ──────────────────────────────────────────────────────────
class BLEServer {
public:
    BLEService* createService(BLEUUID uuid) {
        auto* s = new BLEService(uuid);
        services_[uuid.toString()] = s;
        return s;
    }

    BLEService* getServiceByUUID(BLEUUID uuid) {
        auto it = services_.find(uuid.toString());
        return it != services_.end() ? it->second : nullptr;
    }

    void setCallbacks(BLEServerCallbacks* cb) { callbacks_ = cb; }
    BLEServerCallbacks* getCallbacks() const { return callbacks_; }

    BLEAdvertising* getAdvertising();

    int getConnectedCount() const { return connected_count_; }
    size_t getServiceCount() const { return services_.size(); }

    // Test helpers
    void simulateConnect() {
        connected_count_++;
        if (callbacks_) callbacks_->onConnect(this);
    }
    void simulateDisconnect() {
        if (connected_count_ > 0) connected_count_--;
        if (callbacks_) callbacks_->onDisconnect(this);
    }

    ~BLEServer() {
        for (auto& p : services_) delete p.second;
    }

private:
    std::map<std::string, BLEService*> services_;
    BLEServerCallbacks* callbacks_ = nullptr;
    int connected_count_ = 0;
};

// ── Advertising ─────────────────────────────────────────────────────
class BLEAdvertising {
public:
    void addServiceUUID(BLEUUID uuid) { uuids_.push_back(uuid); }
    void setScanResponse(bool v) { scan_response_ = v; }
    void setMinPreferred(uint16_t v) { min_interval_ = v; }
    void setMaxPreferred(uint16_t v) { max_interval_ = v; }

    void start() {
        advertising_ = true;
        fprintf(stderr, "[esp32emu] BLE advertising started (%zu services)\n", uuids_.size());
    }
    void stop() {
        advertising_ = false;
        fprintf(stderr, "[esp32emu] BLE advertising stopped\n");
    }

    bool isAdvertising() const { return advertising_; }
    size_t getServiceUUIDCount() const { return uuids_.size(); }

private:
    std::vector<BLEUUID> uuids_;
    bool advertising_ = false;
    bool scan_response_ = false;
    uint16_t min_interval_ = 0;
    uint16_t max_interval_ = 0;
};

inline BLEAdvertising* BLEServer::getAdvertising() {
    static BLEAdvertising adv;
    return &adv;
}

// ── BLEDevice (static interface) ────────────────────────────────────
class BLEDevice {
public:
    static void init(const std::string& name) {
        device_name_ = name;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] BLE initialized: \"%s\"\n", name.c_str());
    }

    static BLEServer* createServer() {
        if (server_) delete server_;
        server_ = new BLEServer();
        return server_;
    }

    static BLEServer* getServer() { return server_; }
    static std::string getDeviceName() { return device_name_; }
    static bool isInitialized() { return initialized_; }
    static void deinit() {
        if (server_) { delete server_; server_ = nullptr; }
        initialized_ = false;
        device_name_.clear();
    }

    static BLEAdvertising* getAdvertising() {
        if (server_) return server_->getAdvertising();
        static BLEAdvertising fallback;
        return &fallback;
    }

    // Emulated MTU
    static void setMTU(uint16_t mtu) { mtu_ = mtu; }
    static uint16_t getMTU() { return mtu_; }

private:
    static inline std::string device_name_;
    static inline BLEServer* server_ = nullptr;
    static inline bool initialized_ = false;
    static inline uint16_t mtu_ = 517;
};

// ── Test namespace ──────────────────────────────────────────────────
namespace ble_test {
    inline void reset() {
        BLEDevice::deinit();
    }
}
