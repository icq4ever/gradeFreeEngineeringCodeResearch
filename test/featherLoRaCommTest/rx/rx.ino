#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS	8
#define RFM95_RST	4
#define RFM95_INT	7

#define RF95_FREQ	433.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define IN_MESSAGE_SIZE			10
#define OUT_MESSAGE_SIZE		2
#define RECEIVE_BUFFER_SIZE		IN_MESSAGE_SIZE+2
#define SEND_BUFFER_SIZE		OUT_MESSAGE_SIZE+2

uint8_t getBuffer[RECEIVE_BUFFER_SIZE];
// uint8_t receiveLen = sizeof(getBuffer);

boolean btnStatus[IN_MESSAGE_SIZE];


void setup(){
	pinMode(RFM95_RST, OUTPUT);
	digitalWrite(RFM95_RST, HIGH);

	// while(!Sxerial);
	Serial.begin(115200);
	delay(100);
	
	pinMode(13, OUTPUT);

	initLoRa();

	for(int i=0; i<IN_MESSAGE_SIZE; i++){
		btnStatus[i] = false;
	}
}

	
void loop(){
	if(rf95.available()){
		receiveFromControlModule();
	}
		
	getBtnStatus();
	printBtnStatus();	
}

void initLoRa(){
	Serial.println("Feather LoRa RX Test!");

	// manual LoRa reset
	digitalWrite(RFM95_RST, LOW);
	delay(1000);
	digitalWrite(RFM95_RST, HIGH);
	delay(1000);

	while(!rf95.init()){
		Serial.println("LoRa radio init failed");
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
	rf95.setTxPower(5, false);
}

void sendToControlModule(){
	// uint8_t
}
void receiveFromControlModule(){
	uint8_t getBufferLen = sizeof(getBuffer);
	
	// if(rf95.recv((uint8_t *)getBuffer, RECEIVE_BUFFER_SIZE)){
	if(rf95.recv((char *)getBuffer, &getBufferLen)){
		// digitalWrite(13, HIGH);

		rf95.waitPacketSent();
		
		// digitalWrite(13, LOW);
	} else {
		Serial.println("Receive failed");
	}
	
}

void getBtnStatus(){
	if(getBuffer[0] == '/') {
		digitalWrite(13, HIGH);
		for(int i=1; i<RECEIVE_BUFFER_SIZE-1; i++){
			if((char)getBuffer[i] == '1')		btnStatus[i-1] = true;
			else								btnStatus[i-1] = false;
		}
		digitalWrite(13, LOW);
	} 
}

void printBtnStatus(){
	Serial.print("/");
	for(int i=0; i<IN_MESSAGE_SIZE; i++){
		if(btnStatus[i])	Serial.print("1");
		else				Serial.print("0");

		if(i!=IN_MESSAGE_SIZE-1)	Serial.print(",");
	}
	Serial.println();
}
void sendToP5(){}