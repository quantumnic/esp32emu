// Hardware timer example using ESP-IDF timer driver
// Demonstrates timer group configuration and counter reading

#include <Arduino.h>
#include "driver/timer.h"

void setup() {
    Serial.begin(115200);
    Serial.println("=== Hardware Timer Demo ===");

    // Configure timer: 80MHz / 80 = 1MHz (1us per tick)
    timer_config_t config{};
    config.divider = 80;
    config.counter_dir = TIMER_COUNT_UP;
    config.alarm_en = TIMER_ALARM_DIS;
    config.auto_reload = TIMER_AUTORELOAD_DIS;

    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_start(TIMER_GROUP_0, TIMER_0);

    Serial.println("Timer started (1MHz, counting up)");

    // Set alarm at 1 second (1,000,000 ticks)
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000000);
    timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN);
    Serial.println("Alarm set at 1,000,000 ticks (1 second)");
}

void loop() {
    uint64_t timer_val;
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_val);

    Serial.print("⏱️ Timer: ");
    Serial.print((unsigned long)(timer_val / 1000));
    Serial.print(" ms (");
    Serial.print((unsigned long)timer_val);
    Serial.println(" ticks)");

    auto& t = esp32emu::timer_drv::getTimer(TIMER_GROUP_0, TIMER_0);
    if (t.alarm_enabled && timer_val >= t.alarm_value) {
        Serial.println("🔔 ALARM triggered!");
        timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_DIS);

        // Reset and restart
        timer_pause(TIMER_GROUP_0, TIMER_0);
        timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
        timer_start(TIMER_GROUP_0, TIMER_0);
        timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN);
    }

    delay(250);
}
