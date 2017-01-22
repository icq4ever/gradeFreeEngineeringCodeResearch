#include <Servo.h>

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
	servoR();
	delay(2000);

	servoStop();
	delay(2000);

	servoL();
	delay(2000);

	servoStop();
	delay(2000);
}

void servoR(){
	myServo.writeMicroseconds(2000);
	Serial.println("R!");
}

void servoL(){
	myServo.writeMicroseconds(1580);
	Serial.println("L!");
}

void servoStop(){
	myServo.writeMicroseconds(1500);
	Serial.println("Stop!");
}
