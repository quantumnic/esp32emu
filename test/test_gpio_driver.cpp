// Test: ESP-IDF GPIO driver mock
#include "driver/gpio.h"
#include <cassert>
#include <cstdio>
#include <atomic>

static std::atomic<int> isrCount{0};

static void gpio_isr(void* arg) {
    (void)arg;
    isrCount++;
}

int main() {
    auto& drv = esp32emu::GpioDriver::instance();
    drv.reset();

    // Basic set direction and level
    assert(gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT) == ESP_OK);
    assert(gpio_set_level(GPIO_NUM_2, 1) == ESP_OK);
    assert(gpio_get_level(GPIO_NUM_2) == 1);
    assert(gpio_set_level(GPIO_NUM_2, 0) == ESP_OK);
    assert(gpio_get_level(GPIO_NUM_2) == 0);

    // Pull mode (no-op but should succeed)
    assert(gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLUP_ONLY) == ESP_OK);

    // ISR handler
    assert(gpio_install_isr_service(0) == ESP_OK);
    assert(gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT) == ESP_OK);
    assert(gpio_set_intr_type(GPIO_NUM_4, GPIO_INTR_POSEDGE) == ESP_OK);
    assert(gpio_isr_handler_add(GPIO_NUM_4, gpio_isr, nullptr) == ESP_OK);

    isrCount = 0;
    // Simulate rising edge
    drv.setLevel(GPIO_NUM_4, 0);
    drv.setLevel(GPIO_NUM_4, 1);
    assert(isrCount == 1);

    // Falling edge should NOT fire on POSEDGE
    drv.setLevel(GPIO_NUM_4, 0);
    assert(isrCount == 1);

    // Change to ANYEDGE
    gpio_set_intr_type(GPIO_NUM_4, GPIO_INTR_ANYEDGE);
    drv.setLevel(GPIO_NUM_4, 1); // rising
    assert(isrCount == 2);
    drv.setLevel(GPIO_NUM_4, 0); // falling
    assert(isrCount == 3);

    // Remove handler
    gpio_isr_handler_remove(GPIO_NUM_4);
    drv.setLevel(GPIO_NUM_4, 1);
    assert(isrCount == 3); // no change

    // State inspection
    auto state = drv.getState(GPIO_NUM_2);
    assert(state.mode == GPIO_MODE_OUTPUT);

    printf("test_gpio_driver: all assertions passed\n");
    return 0;
}
