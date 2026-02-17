// Servo 0-180° Sweep with Terminal Visualization
// esp32emu run --board uno examples/servo_sweep/servo_sweep.ino
#include "Arduino.h"
#include "Servo.h"

Servo myServo;
int angle = 0;
int direction = 1;

void setup() {
    Serial.begin(115200);
    myServo.attach(9);
    Serial.println("🔄 Servo Sweep — 0° to 180° and back");
}

void loop() {
    myServo.write(angle);
    
    angle += direction * 5;
    if (angle >= 180) { angle = 180; direction = -1; }
    if (angle <= 0)   { angle = 0;   direction = 1; }
    
    delay(50);
}
