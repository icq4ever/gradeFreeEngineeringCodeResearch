

// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX

#include <SPI.h>
#include <RH_RF95.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver();

// Change to 433.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
int LED=13;

#define SERVOMIN 205 // 4096 * (1/20)
#define SERVOMIDDLE 308 // 4096 * (1.5 /20)
#define SERVOMAX 410 // 4096 * (2 / 20)

int throttleValue;
int handlingValue;
int throttlingCounter = 0;
int hanglingCounter = 0;

unsigned int servoAngle = SERVOMIDDLE;
unsigned int boostAngle = SERVOMIDDLE;

void setup() {
	pinMode(LED, OUTPUT);     
	pinMode(RFM95_RST, OUTPUT);
	digitalWrite(RFM95_RST, HIGH);


	servo.begin();
	servo.setPWMFreq(50);

	servo.setPWM(0,0,SERVOMIDDLE);
	servo.setPWM(1,0,SERVOMIDDLE);

  //while (!Serial);
	Serial.begin(9600);
	delay(100);

	Serial.println("Feather LoRa RX Test!");

  // manual reset
	digitalWrite(RFM95_RST, LOW);
	delay(10);
	digitalWrite(RFM95_RST, HIGH);
	delay(10);

	while (!rf95.init()) {
		Serial.println("LoRa radio init failed");
		while (1);
	}
	Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
	if (!rf95.setFrequency(RF95_FREQ)) {
		Serial.println("setFrequency failed");
		while (1);
	}
	Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
	rf95.setTxPower(23, false);
}

void loop() {
	if (rf95.available()) {
    // Should be a message for us now   
		uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
		uint8_t len = sizeof(buf);

		if (rf95.recv(buf, &len)) {
			if(buf[0] == '/'){
				throttleValue = int(buf[1])-100;
				handlingValue = int(buf[2])-100;

				boostAngle = int(map(throttleValue, -100, 100, SERVOMIN, SERVOMAX));
				servoAngle = int(map(handlingValue, 100, -100, SERVOMIN, SERVOMAX));

				if(boostAngle == 307)	boostAngle = 308;
				if(servoAngle == 307)	servoAngle = 308;
			}


			

			// Serial.print(boostAngle);
			// Serial.print(",");
			// Serial.println(servoAngle);
		} else 	{
			Serial.println("Receive failed");
		}
	}

	servo.setPWM(0, 0, boostAngle);
	servo.setPWM(1, 0, servoAngle);
}