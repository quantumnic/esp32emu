#include <Arduino.h>
#include <Si5351.h>

Si5351 clockgen;

void setup() {
    Serial.begin(115200);
    Serial.println("Si5351 VFO — Variable Frequency Oscillator");

    clockgen.init(0, 25000000);

    // CLK0: 7.1 MHz (40m ham band)
    clockgen.set_freq(710000000ULL, SI5351_CLK0);
    clockgen.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);

    // CLK1: 14.2 MHz (20m ham band)
    clockgen.set_freq(1420000000ULL, SI5351_CLK1);
    clockgen.drive_strength(SI5351_CLK1, SI5351_DRIVE_4MA);

    // CLK2: disabled
    clockgen.output_enable(SI5351_CLK2, false);

    Serial.println("VFO outputs active.");
}

void loop() {
    // Tune CLK0 up by 100 Hz each second
    static uint64_t freq = 710000000ULL;
    freq += 10000ULL; // +100 Hz (centi-Hz units)
    clockgen.set_freq(freq, SI5351_CLK0);

    Serial.print("CLK0: ");
    Serial.print((unsigned long)(freq / 100));
    Serial.println(" Hz");

    delay(1000);
}
