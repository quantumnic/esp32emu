// TinyGPS++ GPS Tracker Example
// Parses NMEA data and displays location

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;
SoftwareSerial gpsSerial(16, 17); // RX, TX

void displayInfo() {
    if (gps.location.isValid()) {
        Serial.print("Location: ");
        Serial.print(gps.location.lat(), 6);
        Serial.print(", ");
        Serial.println(gps.location.lng(), 6);
    } else {
        Serial.println("Location: INVALID");
    }

    if (gps.date.isValid()) {
        Serial.printf("Date: %02d/%02d/%04d\n",
                       gps.date.month(), gps.date.day(), gps.date.year());
    }

    if (gps.time.isValid()) {
        Serial.printf("Time: %02d:%02d:%02d\n",
                       gps.time.hour(), gps.time.minute(), gps.time.second());
    }

    if (gps.speed.isValid()) {
        Serial.print("Speed: ");
        Serial.print(gps.speed.kmph());
        Serial.println(" km/h");
    }

    if (gps.altitude.isValid()) {
        Serial.print("Altitude: ");
        Serial.print(gps.altitude.meters());
        Serial.println(" m");
    }

    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600);
    Serial.println("TinyGPS++ Tracker");
    Serial.println("=================");
}

void loop() {
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    static unsigned long lastDisplay = 0;
    if (millis() - lastDisplay > 2000) {
        lastDisplay = millis();
        displayInfo();
    }
}
