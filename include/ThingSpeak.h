// ThingSpeak IoT cloud client mock for esp32emu
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>

// Status codes
#define TS_OK_SUCCESS             200
#define TS_ERR_BADAPIKEY          -1
#define TS_ERR_BADURL             -2
#define TS_ERR_OUT_OF_RANGE       -3
#define TS_ERR_INVALID_FIELD_NUM  -4
#define TS_ERR_SETFIELD_NOT_CALLED -5
#define TS_ERR_NOT_INSERTED       -6
#define TS_ERR_CONNECT_FAILED     -7

class ThingSpeakClass {
public:
    bool begin(void* /*client*/, const char* server = "api.thingspeak.com", uint16_t port = 80) {
        server_ = server;
        port_ = port;
        begun_ = true;
        fprintf(stderr, "[esp32emu] ThingSpeak: begin(%s:%d)\n", server, port);
        return true;
    }

    int setField(unsigned int field, int value) { return setField(field, std::to_string(value)); }
    int setField(unsigned int field, long value) { return setField(field, std::to_string(value)); }
    int setField(unsigned int field, float value) { return setField(field, std::to_string(value)); }
    int setField(unsigned int field, double value) { return setField(field, std::to_string(value)); }
    int setField(unsigned int field, const char* value) { return setField(field, std::string(value)); }

    int setField(unsigned int field, const std::string& value) {
        if (field < 1 || field > 8) return TS_ERR_INVALID_FIELD_NUM;
        fields_[field] = value;
        fprintf(stderr, "[esp32emu] ThingSpeak: setField(%u, \"%s\")\n", field, value.c_str());
        return TS_OK_SUCCESS;
    }

    int setLatitude(float lat) { lat_ = lat; return TS_OK_SUCCESS; }
    int setLongitude(float lon) { lon_ = lon; return TS_OK_SUCCESS; }
    int setElevation(float elev) { elev_ = elev; return TS_OK_SUCCESS; }
    int setStatus(const char* status) { status_ = status; return TS_OK_SUCCESS; }

    int writeFields(unsigned long channelNumber, const char* writeAPIKey) {
        channel_ = channelNumber;
        write_key_ = writeAPIKey;
        write_count_++;
        fprintf(stderr, "[esp32emu] ThingSpeak: writeFields(channel=%lu) — %zu fields\n",
                channelNumber, fields_.size());
        last_write_fields_ = fields_;
        fields_.clear();
        status_.clear();
        return inject_write_status_;
    }

    int writeField(unsigned long channelNumber, unsigned int field, float value, const char* writeAPIKey) {
        setField(field, value);
        return writeFields(channelNumber, writeAPIKey);
    }

    int writeField(unsigned long channelNumber, unsigned int field, int value, const char* writeAPIKey) {
        setField(field, value);
        return writeFields(channelNumber, writeAPIKey);
    }

    int writeField(unsigned long channelNumber, unsigned int field, const char* value, const char* writeAPIKey) {
        setField(field, value);
        return writeFields(channelNumber, writeAPIKey);
    }

    float readFloatField(unsigned long channelNumber, unsigned int field, const char* readAPIKey = nullptr) {
        (void)channelNumber; (void)readAPIKey;
        read_count_++;
        fprintf(stderr, "[esp32emu] ThingSpeak: readFloatField(channel=%lu, field=%u)\n", channelNumber, field);
        auto it = inject_read_values_.find(field);
        return it != inject_read_values_.end() ? std::stof(it->second) : 0.0f;
    }

    long readLongField(unsigned long channelNumber, unsigned int field, const char* readAPIKey = nullptr) {
        return (long)readFloatField(channelNumber, field, readAPIKey);
    }

    std::string readStringField(unsigned long channelNumber, unsigned int field, const char* readAPIKey = nullptr) {
        (void)channelNumber; (void)readAPIKey;
        read_count_++;
        auto it = inject_read_values_.find(field);
        return it != inject_read_values_.end() ? it->second : "";
    }

    int readMultipleFields(unsigned long channelNumber, const char* readAPIKey = nullptr) {
        (void)channelNumber; (void)readAPIKey;
        read_count_++;
        fprintf(stderr, "[esp32emu] ThingSpeak: readMultipleFields(channel=%lu)\n", channelNumber);
        return inject_write_status_;
    }

    int getLastReadStatus() const { return last_read_status_; }

    // ── Test helpers ──
    void injectWriteStatus(int status) { inject_write_status_ = status; }
    void injectReadValue(unsigned int field, const std::string& val) { inject_read_values_[field] = val; }
    void injectReadStatus(int status) { last_read_status_ = status; }

    uint32_t getWriteCount() const { return write_count_; }
    uint32_t getReadCount() const { return read_count_; }
    unsigned long getChannel() const { return channel_; }
    std::string getWriteKey() const { return write_key_; }
    bool isBegun() const { return begun_; }
    const std::map<unsigned int, std::string>& getLastWriteFields() const { return last_write_fields_; }

    void reset() {
        fields_.clear();
        last_write_fields_.clear();
        inject_read_values_.clear();
        write_count_ = 0;
        read_count_ = 0;
        inject_write_status_ = TS_OK_SUCCESS;
        last_read_status_ = TS_OK_SUCCESS;
    }

private:
    std::string server_;
    uint16_t port_ = 80;
    bool begun_ = false;
    unsigned long channel_ = 0;
    std::string write_key_;
    std::map<unsigned int, std::string> fields_;
    std::map<unsigned int, std::string> last_write_fields_;
    std::map<unsigned int, std::string> inject_read_values_;
    uint32_t write_count_ = 0;
    uint32_t read_count_ = 0;
    int inject_write_status_ = TS_OK_SUCCESS;
    int last_read_status_ = TS_OK_SUCCESS;
    float lat_ = 0, lon_ = 0, elev_ = 0;
    std::string status_;
};

extern ThingSpeakClass ThingSpeak;

// Inline global instance
inline ThingSpeakClass ThingSpeak;
