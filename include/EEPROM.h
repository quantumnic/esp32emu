#pragma once
#include <cstdint>
#include <cstring>
#include <vector>

class EEPROMClass {
public:
    void begin(size_t size) {
        data_.resize(size, 0);
    }

    uint8_t read(int addr) {
        if (addr < 0 || addr >= (int)data_.size()) return 0;
        return data_[addr];
    }

    void write(int addr, uint8_t val) {
        if (addr >= 0 && addr < (int)data_.size())
            data_[addr] = val;
    }

    bool commit() { return true; }
    void end() {}

    uint8_t& operator[](int addr) { return data_[addr]; }

    template<typename T>
    T get(int addr, T& t) {
        if (addr >= 0 && addr + (int)sizeof(T) <= (int)data_.size())
            memcpy(&t, &data_[addr], sizeof(T));
        return t;
    }

    template<typename T>
    void put(int addr, const T& t) {
        if (addr >= 0 && addr + (int)sizeof(T) <= (int)data_.size())
            memcpy(&data_[addr], &t, sizeof(T));
    }

    size_t length() const { return data_.size(); }

private:
    std::vector<uint8_t> data_;
};

extern EEPROMClass EEPROM;
