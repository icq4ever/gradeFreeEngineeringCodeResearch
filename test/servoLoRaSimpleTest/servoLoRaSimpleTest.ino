#include <SPI.h>
#include <RH_RF95.h>
#include <Servo.h>
#include <Adafruit_MAX31856.h>

int motor_pin = 11;
int iDirection = -1;
int directionStop = 0;

const int RFM95_CS			= 8;
const int RFM95_RST			= 4;
const int RFM95_INT			= 7;
const float RF95_FREQ		= 433.0;
RH_RF95 rf95(RFM95_CS, RFM95_INT);


Servo myServo;

// _servoDirection < 0  : left
// _servoDirection == 0 : stop
// _servoDirection > 0  : right
// void controlServoDirection(int _servoDirection);

void initLoRa() { // init
	pinMode(RFM95_RST, OUTPUT);
	digitalWrite(RFM95_RST, HIGH);

	Serial.println("Feather LoRa RX Test!");

	// manual LoRa reset
	digitalWrite(RFM95_RST, LOW);
	delay(10);
	digitalWrite(RFM95_RST, HIGH);
	delay(10);

	while(!rf95.init()){
		Serial.println("LoRa radio init faild");
		while(1);
	}

	Serial.println("LoRa radio init OK!");

	// defauls after init are 434.0MHz. 
	if(!rf95.setFrequency(RF95_FREQ)){
		Serial.println("setFrequency failed.");
		while(1);
	}

	Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

	// The default transmitter power is 13dBm, using PA_BOOST.
	// If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
	// you can set transmitter powers from 5 to 23 dBm:
	rf95.setTxPower(23, false);
}

void setup() {
	myServo.attach(motor_pin);
	Serial.begin(115200);

	initLoRa();
}
void loop() {
	
	receiveMessage();
}

void receiveMessage(){
	if(rf95.available()){
		// uint8_t recvB[10];
		// uint8_t bufL = sizeof(recvB);

		// if(rf95.recv(recvB, &bufL)){
		// 	// recv ok
		// 	if(recvB[0] == '/'){
		// 		// servoStop();
		// 	} else {
		// 	// recv failed
		// 	}
		// }
	}
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