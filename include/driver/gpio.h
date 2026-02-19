// esp32emu – ESP-IDF GPIO driver mock
#pragma once
#include <cstdint>
#include <cstdio>
#include <functional>
#include <map>
#include <mutex>

typedef int gpio_num_t;
typedef int gpio_mode_t;
typedef int gpio_pull_mode_t;
typedef int gpio_int_type_t;

#define GPIO_NUM_0   0
#define GPIO_NUM_1   1
#define GPIO_NUM_2   2
#define GPIO_NUM_4   4
#define GPIO_NUM_5   5
#define GPIO_NUM_12  12
#define GPIO_NUM_13  13
#define GPIO_NUM_14  14
#define GPIO_NUM_15  15
#define GPIO_NUM_16  16
#define GPIO_NUM_17  17
#define GPIO_NUM_18  18
#define GPIO_NUM_19  19
#define GPIO_NUM_21  21
#define GPIO_NUM_22  22
#define GPIO_NUM_23  23
#define GPIO_NUM_25  25
#define GPIO_NUM_26  26
#define GPIO_NUM_27  27
#define GPIO_NUM_32  32
#define GPIO_NUM_33  33
#define GPIO_NUM_34  34
#define GPIO_NUM_35  35
#define GPIO_NUM_36  36
#define GPIO_NUM_39  39

#define GPIO_MODE_DISABLE       0
#define GPIO_MODE_INPUT         1
#define GPIO_MODE_OUTPUT        2
#define GPIO_MODE_OUTPUT_OD     3
#define GPIO_MODE_INPUT_OUTPUT  4
#define GPIO_MODE_INPUT_OUTPUT_OD 5

#define GPIO_PULLUP_ONLY    0
#define GPIO_PULLDOWN_ONLY  1
#define GPIO_PULLUP_PULLDOWN 2
#define GPIO_FLOATING       3

#define GPIO_INTR_DISABLE    0
#define GPIO_INTR_POSEDGE   1
#define GPIO_INTR_NEGEDGE   2
#define GPIO_INTR_ANYEDGE   3
#define GPIO_INTR_LOW_LEVEL 4
#define GPIO_INTR_HIGH_LEVEL 5

#define ESP_OK   0
#define ESP_FAIL -1
#define ESP_ERR_INVALID_ARG -2

typedef int esp_err_t;
typedef void (*gpio_isr_t)(void* arg);

namespace esp32emu {

struct GpioState {
    gpio_mode_t mode = GPIO_MODE_DISABLE;
    int level = 0;
    gpio_int_type_t intrType = GPIO_INTR_DISABLE;
    gpio_isr_t isrHandler = nullptr;
    void* isrArg = nullptr;
};

class GpioDriver {
public:
    static GpioDriver& instance() {
        static GpioDriver inst;
        return inst;
    }

    esp_err_t setDirection(gpio_num_t pin, gpio_mode_t mode) {
        std::lock_guard<std::mutex> lk(mu_);
        pins_[pin].mode = mode;
        return ESP_OK;
    }

    esp_err_t setLevel(gpio_num_t pin, uint32_t level) {
        std::lock_guard<std::mutex> lk(mu_);
        auto& s = pins_[pin];
        int old = s.level;
        s.level = level ? 1 : 0;
        if (s.isrHandler && s.intrType != GPIO_INTR_DISABLE) {
            bool fire = false;
            if (s.intrType == GPIO_INTR_POSEDGE && old == 0 && s.level == 1) fire = true;
            if (s.intrType == GPIO_INTR_NEGEDGE && old == 1 && s.level == 0) fire = true;
            if (s.intrType == GPIO_INTR_ANYEDGE && old != s.level) fire = true;
            if (fire) {
                auto h = s.isrHandler;
                auto a = s.isrArg;
                lk.~lock_guard();
                h(a);
            }
        }
        return ESP_OK;
    }

    int getLevel(gpio_num_t pin) {
        std::lock_guard<std::mutex> lk(mu_);
        return pins_[pin].level;
    }

    esp_err_t setIntrType(gpio_num_t pin, gpio_int_type_t type) {
        std::lock_guard<std::mutex> lk(mu_);
        pins_[pin].intrType = type;
        return ESP_OK;
    }

    esp_err_t isrHandlerAdd(gpio_num_t pin, gpio_isr_t handler, void* arg) {
        std::lock_guard<std::mutex> lk(mu_);
        pins_[pin].isrHandler = handler;
        pins_[pin].isrArg = arg;
        return ESP_OK;
    }

    esp_err_t isrHandlerRemove(gpio_num_t pin) {
        std::lock_guard<std::mutex> lk(mu_);
        pins_[pin].isrHandler = nullptr;
        pins_[pin].isrArg = nullptr;
        return ESP_OK;
    }

    void reset() {
        std::lock_guard<std::mutex> lk(mu_);
        pins_.clear();
    }

    GpioState getState(gpio_num_t pin) {
        std::lock_guard<std::mutex> lk(mu_);
        return pins_[pin];
    }

private:
    std::mutex mu_;
    std::map<int, GpioState> pins_;
};

} // namespace esp32emu

// ESP-IDF style API
inline esp_err_t gpio_set_direction(gpio_num_t pin, gpio_mode_t mode) {
    return esp32emu::GpioDriver::instance().setDirection(pin, mode);
}

inline esp_err_t gpio_set_level(gpio_num_t pin, uint32_t level) {
    return esp32emu::GpioDriver::instance().setLevel(pin, level);
}

inline int gpio_get_level(gpio_num_t pin) {
    return esp32emu::GpioDriver::instance().getLevel(pin);
}

inline esp_err_t gpio_set_intr_type(gpio_num_t pin, gpio_int_type_t type) {
    return esp32emu::GpioDriver::instance().setIntrType(pin, type);
}

inline esp_err_t gpio_install_isr_service(int flags) {
    (void)flags;
    return ESP_OK;
}

inline esp_err_t gpio_isr_handler_add(gpio_num_t pin, gpio_isr_t handler, void* arg) {
    return esp32emu::GpioDriver::instance().isrHandlerAdd(pin, handler, arg);
}

inline esp_err_t gpio_isr_handler_remove(gpio_num_t pin) {
    return esp32emu::GpioDriver::instance().isrHandlerRemove(pin);
}

inline esp_err_t gpio_set_pull_mode(gpio_num_t pin, gpio_pull_mode_t mode) {
    (void)pin; (void)mode;
    return ESP_OK;
}

inline esp_err_t gpio_reset_pin(gpio_num_t pin) {
    esp32emu::GpioDriver::instance().reset();
    (void)pin;
    return ESP_OK;
}
