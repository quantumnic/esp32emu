// ThingSpeak weather station — upload temperature + humidity readings
#include <WiFi.h>
#include <ThingSpeak.h>

const char* ssid = "MyNetwork";
const char* password = "MyPassword";
unsigned long channelID = 123456;
const char* writeKey = "ABCDEFGHIJKLMNOP";

WiFiClient client;

void setup() {
    Serial.begin(115200);
    Serial.println("ThingSpeak Weather Station");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    ThingSpeak.begin(&client);
}

void loop() {
    // Simulated sensor readings
    float temperature = 22.5 + (millis() % 100) * 0.1;
    float humidity = 45.0 + (millis() % 50) * 0.2;
    float pressure = 1013.25;

    Serial.printf("Temp: %.1f°C  Humidity: %.1f%%  Pressure: %.1f hPa\n",
                  temperature, humidity, pressure);

    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, pressure);
    ThingSpeak.setStatus("online");

    int status = ThingSpeak.writeFields(channelID, writeKey);
    if (status == 200) {
        Serial.println("ThingSpeak update OK");
    } else {
        Serial.printf("ThingSpeak error: %d\n", status);
    }

    delay(20000); // ThingSpeak rate limit: 15s
}
