#pragma once
#include <cstdint>
#include <string>

// SparkFun APDS-9960 gesture/proximity/ambient light/color sensor mock
class APDS9960 {
public:
    APDS9960() = default;

    bool init() { initialized_ = true; return true; }
    bool isGestureAvailable() { return gesture_available_; }

    // Gesture: 0=none, 1=up, 2=down, 3=left, 4=right, 5=near, 6=far
    int readGesture() {
        if (!gesture_available_) return 0;
        gesture_available_ = false;
        return gesture_;
    }

    bool enableGestureSensor(bool /*interrupts*/ = true) { gesture_enabled_ = true; return true; }
    bool disableGestureSensor() { gesture_enabled_ = false; return true; }

    bool enableProximitySensor(bool /*interrupts*/ = false) { proximity_enabled_ = true; return true; }
    bool disableProximitySensor() { proximity_enabled_ = false; return true; }
    bool readProximity(uint8_t &val) { val = proximity_; return true; }

    bool enableLightSensor(bool /*interrupts*/ = false) { light_enabled_ = true; return true; }
    bool disableLightSensor() { light_enabled_ = false; return true; }
    bool readAmbientLight(uint16_t &val) { val = ambient_light_; return true; }
    bool readRedLight(uint16_t &val) { val = red_; return true; }
    bool readGreenLight(uint16_t &val) { val = green_; return true; }
    bool readBlueLight(uint16_t &val) { val = blue_; return true; }

    // Setters for test injection
    void setGesture(int g) { gesture_ = g; gesture_available_ = true; }
    void setProximity(uint8_t p) { proximity_ = p; }
    void setAmbientLight(uint16_t a) { ambient_light_ = a; }
    void setColor(uint16_t r, uint16_t g, uint16_t b) { red_ = r; green_ = g; blue_ = b; }

    bool isInitialized() const { return initialized_; }

    // Gesture constants
    static constexpr int DIR_NONE  = 0;
    static constexpr int DIR_UP    = 1;
    static constexpr int DIR_DOWN  = 2;
    static constexpr int DIR_LEFT  = 3;
    static constexpr int DIR_RIGHT = 4;
    static constexpr int DIR_NEAR  = 5;
    static constexpr int DIR_FAR   = 6;

private:
    bool initialized_ = false;
    bool gesture_enabled_ = false;
    bool proximity_enabled_ = false;
    bool light_enabled_ = false;
    bool gesture_available_ = false;
    int gesture_ = 0;
    uint8_t proximity_ = 0;
    uint16_t ambient_light_ = 0;
    uint16_t red_ = 0, green_ = 0, blue_ = 0;
};
