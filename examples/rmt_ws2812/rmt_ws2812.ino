// RMT WS2812B LED driver — uses RMT peripheral for precise timing
#include <driver/rmt.h>

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define RMT_TX_GPIO    18
#define NUM_LEDS       8

// WS2812B timing (in 0.1us ticks at clk_div=8, 80MHz APB)
#define T0H 4   // 0.4us
#define T0L 9   // 0.9us
#define T1H 8   // 0.8us
#define T1L 5   // 0.5us

struct RGB { uint8_t r, g, b; };
RGB leds[NUM_LEDS];

void ws2812_init() {
    rmt_config_t config{};
    config.rmt_mode = RMT_MODE_TX;
    config.channel = RMT_TX_CHANNEL;
    config.gpio_num = RMT_TX_GPIO;
    config.clk_div = 8;
    config.mem_block_num = 1;
    config.tx_config.idle_output_en = true;
    config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;

    rmt_config(&config);
    rmt_driver_install(RMT_TX_CHANNEL, 0, 0);
}

void ws2812_show() {
    // 24 bits per LED (GRB order)
    rmt_item32_t items[NUM_LEDS * 24 + 1];
    int idx = 0;

    for (int led = 0; led < NUM_LEDS; led++) {
        uint8_t bytes[3] = { leds[led].g, leds[led].r, leds[led].b };
        for (int b = 0; b < 3; b++) {
            for (int bit = 7; bit >= 0; bit--) {
                if (bytes[b] & (1 << bit)) {
                    items[idx].level0 = 1; items[idx].duration0 = T1H;
                    items[idx].level1 = 0; items[idx].duration1 = T1L;
                } else {
                    items[idx].level0 = 1; items[idx].duration0 = T0H;
                    items[idx].level1 = 0; items[idx].duration1 = T0L;
                }
                idx++;
            }
        }
    }
    items[idx].val = 0; // reset code
    idx++;

    rmt_write_items(RMT_TX_CHANNEL, items, idx, true);
    rmt_wait_tx_done(RMT_TX_CHANNEL, 100);
}

void setup() {
    Serial.begin(115200);
    Serial.println("RMT WS2812B LED Strip");
    ws2812_init();
}

void loop() {
    // Rainbow cycle
    static uint8_t hue = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t h = hue + (i * 256 / NUM_LEDS);
        // Simple HSV→RGB (hue only, full saturation/value)
        uint8_t region = h / 43;
        uint8_t frac = (h - region * 43) * 6;
        switch (region) {
            case 0: leds[i] = {255, frac, 0}; break;
            case 1: leds[i] = {255 - frac, 255, 0}; break;
            case 2: leds[i] = {0, 255, frac}; break;
            case 3: leds[i] = {0, 255 - frac, 255}; break;
            case 4: leds[i] = {frac, 0, 255}; break;
            default: leds[i] = {255, 0, 255 - frac}; break;
        }
    }
    ws2812_show();

    Serial.print("🌈 Rainbow hue=");
    Serial.println(hue);
    hue++;
    delay(50);
}
