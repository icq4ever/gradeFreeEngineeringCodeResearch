#include <SPI.h>
#include <RH_RF95.h>
#include <Servo.h>
#include <Adafruit_MAX31856.h>

int motor_pin = 9;
int iDirection = -1;
int directionStop = 0;
Servo myServo;

// _servoDirection < 0  : left
// _servoDirection == 0 : stop
// _servoDirection > 0  : right
// void controlServoDirection(int _servoDirection);

void setup() {
	myServo.attach(motor_pin);
	Serial.begin(9600);
}
void loop() {
	
	servoStop();
}

void servoR(){
	myServo.writeMicroseconds(2000);
	Serial.println("R!");
}

void servoL(){
	myServo.writeMicroseconds(1000);
	Serial.println("L!");
}

void servoStop(){
	myServo.writeMicroseconds(1500);
	Serial.println("Stop!");
}
