// GPS Tracker example — reads NMEA from Serial, displays position
#include "Arduino.h"
#include "TinyGPSPlus.h"

TinyGPSPlus gps;

void setup() {
    Serial.begin(115200);
    Serial.println("GPS Tracker starting...");

    // Inject test NMEA data (in real hardware, Serial1 would provide this)
    gps.setLocation(47.3769, 8.5417);  // Zurich
    gps.setAltitude(408.0);
    gps.setSatellites(9);
    gps.setSpeed(5.0);
}

void loop() {
    if (gps.location.isValid()) {
        Serial.print("Lat: "); Serial.println(gps.location.lat());
        Serial.print("Lng: "); Serial.println(gps.location.lng());
        Serial.print("Alt: "); Serial.print(gps.altitude.meters()); Serial.println(" m");
        Serial.print("Sats: "); Serial.println(gps.satellites.value());
        Serial.print("Speed: "); Serial.print(gps.speed.kmph()); Serial.println(" km/h");

        // Distance to Bern
        double dist = TinyGPSPlus::distanceBetween(
            gps.location.lat(), gps.location.lng(),
            46.9480, 7.4474);
        Serial.print("Distance to Bern: "); Serial.print(dist / 1000.0); Serial.println(" km");
    } else {
        Serial.println("Waiting for GPS fix...");
    }
    delay(2000);
}
