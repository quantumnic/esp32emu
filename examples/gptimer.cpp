// Example: ESP-IDF v5 GPTimer — periodic alarm with auto-reload
#include "Arduino.h"
#include "driver/gptimer.h"

static volatile int alarm_fired = 0;

static bool IRAM_ATTR timer_alarm_cb(gptimer_handle_t /*timer*/, const gptimer_alarm_event_data_t *edata, void * /*ctx*/) {
    alarm_fired++;
    (void)edata;
    return false;  // no need to yield
}

static gptimer_handle_t gptimer = nullptr;

void setup() {
    Serial.begin(115200);
    Serial.println("GPTimer Example — 1 second periodic alarm");

    gptimer_config_t cfg{};
    cfg.clk_src = GPTIMER_CLK_SRC_DEFAULT;
    cfg.direction = GPTIMER_COUNT_UP;
    cfg.resolution_hz = 1000000;  // 1MHz = 1 tick per microsecond

    ESP_ERROR_CHECK(gptimer_new_timer(&cfg, &gptimer));

    gptimer_alarm_config_t alarm_cfg{};
    alarm_cfg.alarm_count = 1000000;  // 1 second
    alarm_cfg.reload_count = 0;
    alarm_cfg.flags.auto_reload_on_alarm = 1;
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_cfg));

    gptimer_event_callbacks_t cbs{};
    cbs.on_alarm = timer_alarm_cb;
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, nullptr));

    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    Serial.println("Timer started!");
}

void loop() {
    static int last_count = 0;
    if (alarm_fired != last_count) {
        Serial.print("⏰ Alarm #");
        Serial.print(alarm_fired);
        Serial.println(" fired!");
        last_count = alarm_fired;
    }
    delay(100);
}
