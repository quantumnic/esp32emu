// esp32emu example — MQ135 Air Quality Sensor
// Read CO2/gas PPM and display air quality status.
#include "Arduino.h"
#include "MQ135.h"

MQ135 gasSensor(34);  // Analog pin

void setup() {
    Serial.begin(115200);
    Serial.println("MQ135 Air Quality Monitor");
    Serial.println("Warming up sensor...");
    gasSensor._injectRaw(300);  // Simulate a reading
}

void loop() {
    float resistance = gasSensor.getResistance();
    float ppm = gasSensor.getPPM();
    float cppm = gasSensor.getCorrectedPPM(22.0, 50.0);
    float rzero = gasSensor.getRZero();

    Serial.printf("R: %.1f kΩ | RZero: %.1f kΩ\n", resistance, rzero);
    Serial.printf("PPM: %.0f | Corrected PPM (22°C/50%%RH): %.0f\n", ppm, cppm);

    const char* quality;
    if (cppm < 400) quality = "🟢 Excellent";
    else if (cppm < 600) quality = "🟡 Good";
    else if (cppm < 1000) quality = "🟠 Moderate";
    else quality = "🔴 Poor";
    Serial.printf("Air Quality: %s\n\n", quality);

    delay(2000);
}
