// RTClib DS3231 RTC example — set time, read, check lost power
#include <Arduino.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
    Serial.begin(115200);
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC!");
        while (1) delay(10);
    }

    if (rtc.lostPower()) {
        Serial.println("RTC lost power, setting time...");
        rtc.adjust(DateTime(2026, 2, 23, 4, 0, 0));
    }

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");
}

void loop() {
    DateTime now = rtc.now();
    Serial.print(now.year()); Serial.print('/');
    Serial.print(now.month()); Serial.print('/');
    Serial.print(now.day()); Serial.print(' ');
    Serial.print(now.hour()); Serial.print(':');
    Serial.print(now.minute()); Serial.print(':');
    Serial.println(now.second());
    delay(1000);
}
