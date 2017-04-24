     // Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

#include <SPI.h>
#include <RH_RF95.h>

/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

#define SERVOMIN 205 // 4096 * (1/20)
#define SERVOMIDDLE 308 // 4096 * (1.5 /20)
#define SERVOMAX 410 // 4096 * (2 / 20)

// Change to 433.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0

#define RADIOPACKETSIZE 20

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int status_led = 13;
char radiopacket[RADIOPACKETSIZE] ={0};

int bCupIsEmpty = 1;
int handInHandRate = 0;
int bHandOn = 0;

int throttleValue = 100;
int handlingValue = 100;

// safty check
bool RFMessageReady = false;

unsigned long lastPingSentTimer;

void setup() {
	pinMode(13, OUTPUT);
	pinMode(A5, INPUT);        // drinking AMBASA

	pinMode(RFM95_RST, OUTPUT);
	digitalWrite(RFM95_RST, HIGH);

	pinMode(status_led, OUTPUT);
	digitalWrite(status_led, LOW);


	//while (!Serial);
	Serial.begin(9600);
	delay(100);

	// Serial.println("Feather LoRa TX Test!");

	// manual reset
	digitalWrite(RFM95_RST, LOW);
	delay(10);
	digitalWrite(RFM95_RST, HIGH);
	delay(10);

	for(int i=0; i<RADIOPACKETSIZE; i++){
		radiopacket[i] = 0;
	}

	while (!rf95.init()) {
	    // Serial.println("LoRa radio init failed");
	  	while (1);
	}
	// Serial.println("LoRa radio init OK!");

	// Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
	if (!rf95.setFrequency(RF95_FREQ)) {
		// Serial.println("setFrequency failed");
		while (1);
	}
	// Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

	// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

	// The default transmitter power is 13dBm, using PA_BOOST.
	// If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
	// you can set transmitter powers from 5 to 23 dBm:
	rf95.setTxPower(23, false);

	lastPingSentTimer = millis();

}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop(){
	// read message from processing via P5
	while(Serial.available()){
		if(Serial.read() == '/'){
			throttleValue = Serial.read();
			handlingValue = Serial.read();
		}
		RFMessageReady = true;
	}

	// safety check 
	if(RFMessageReady){
		sendMsg();
		sendToP5();
	} else if(millis() - lastPingSentTimer > 100) {
		sendPing();
		lastPingSentTimer = millis();
	}


}

void sendPing(){
	radiopacket[0] = '#';

	rf95.send((uint8_t *)radiopacket, 1);
	delay(10);
}

void sendMsg(){
	radiopacket[0] ='/';
	radiopacket[1] = throttleValue;
	radiopacket[2] = handlingValue;
	

	delay(10);
	rf95.send((uint8_t *)radiopacket, 3);

  // Serial.println("Waiting for packet to complete..."); 
	delay(10);
	RFMessageReady = false;

}

// send to P5
void sendToP5(){
	Serial.print(throttleValue);
	Serial.print(",");
	Serial.print(handlingValue);
	Serial.println();
	Serial.flush();
	delay(10);
}
