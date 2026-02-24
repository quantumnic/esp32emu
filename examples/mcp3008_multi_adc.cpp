#include <Arduino.h>
#include <MCP3008.h>

MCP3008 adc(5);

void setup() {
    Serial.begin(115200);
    Serial.println("MCP3008 Multi-Channel ADC Reader");
    adc.begin();
}

void loop() {
    Serial.println("─────────────────────────────");
    for (uint8_t ch = 0; ch < 8; ch++) {
        uint16_t raw = adc.readADC(ch);
        float voltage = adc.toVoltage(raw, 3.3f);
        Serial.print("CH");
        Serial.print(ch);
        Serial.print(": ");
        Serial.print(raw);
        Serial.print(" (");
        Serial.print(voltage, 3);
        Serial.println("V)");
    }

    // Differential example: CH0 - CH1
    int16_t diff = adc.readADCDifferential(0, 1);
    Serial.print("Diff CH0-CH1: ");
    Serial.println(diff);

    delay(2000);
}
