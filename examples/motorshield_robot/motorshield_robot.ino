// Motor Shield V2 robot demo — drive DC motors forward/backward + stepper positioning
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS;
Adafruit_DCMotor *leftMotor;
Adafruit_DCMotor *rightMotor;
Adafruit_StepperMotor *armStepper;

void setup() {
    Serial.begin(115200);
    Serial.println("Motor Shield Robot Demo");

    AFMS.begin();
    leftMotor = AFMS.getMotor(1);
    rightMotor = AFMS.getMotor(2);
    armStepper = AFMS.getStepper(200, 1);

    armStepper->setSpeed(30);
}

void loop() {
    // Drive forward
    Serial.println("Forward");
    leftMotor->setSpeed(200);
    rightMotor->setSpeed(200);
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    delay(2000);

    // Turn right
    Serial.println("Turn right");
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
    delay(500);

    // Stop and move arm
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
    Serial.println("Arm extending...");
    armStepper->step(50, FORWARD, DOUBLE);
    delay(1000);

    Serial.println("Arm retracting...");
    armStepper->step(50, BACKWARD, DOUBLE);
    delay(1000);

    // Reverse
    Serial.println("Reverse");
    leftMotor->setSpeed(150);
    rightMotor->setSpeed(150);
    leftMotor->run(BACKWARD);
    rightMotor->run(BACKWARD);
    delay(2000);

    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
    armStepper->release();
    delay(3000);
}
