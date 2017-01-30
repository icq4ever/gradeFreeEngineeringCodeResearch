#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS	8
#define RFM95_RST	4
#define RFM95_INT	7

#define RF95_FREQ	433.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);


#define IN_MESSAGE_SIZE			2
#define OUT_MESSAGE_SIZE		10		// button status

#define RECV_BUFFER_SIZE		IN_MESSAGE_SIZE+2
#define SEND_BUFFER_SIZE		OUT_MESSAGE_SIZE+2


bool btnStatus[OUT_MESSAGE_SIZE] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
char sendBuffer[SEND_BUFFER_SIZE];

// actionMessage

void setup(){
	pinMode(RFM95_RST, OUTPUT);
	digitalWrite(RFM95_RST, HIGH);

	// while(!Serial);
	Serial.begin(115200);
	delay(100);
	
	pinMode(13, OUTPUT);

	initLoRa();

	for(int i=0; i<SEND_BUFFER_SIZE; i++){
		sendBuffer[i] = 'N';
	}
}

void loop(){
	// Serial.println("I'm OK");

	sendToActionModule();
	updateSendBuffer();
	Serial.println((char*)sendBuffer);
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

void sendToActionModule(){
	digitalWrite(13, HIGH);

	rf95.send((uint8_t *)sendBuffer, SEND_BUFFER_SIZE);
	rf95.waitPacketSent();
	Serial.println("SENT!");
	digitalWrite(13, LOW);
}

void updateSendBuffer(){
	sendBuffer[0] = '/';
	for(int i=0; i<OUT_MESSAGE_SIZE; i++){
		if(btnStatus[i])		sendBuffer[i+1] = '1';
		else					sendBuffer[i+1] = '0';
	}
	sendBuffer[SEND_BUFFER_SIZE-1] = 0;
}

void receiveFromActionModule(){}

void sendToP5(){}