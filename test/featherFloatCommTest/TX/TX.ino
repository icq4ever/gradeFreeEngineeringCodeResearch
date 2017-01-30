#include <SPI.h>
#include <RH_RF95.h>

const int RFM95_CS		= 8;
const int RFM95_RST		= 4;
const int RFM95_INT		= 7;

const int RF95_FREQ		= 433.0;

RH_RF95 rf95(RFM95_CS, RFM95_INT);

const int OUT_MESSAGE_SIZE = 8;
const int SEND_BUFFER_SIZE = OUT_MESSAGE_SIZE + 2;

const int IN_MESSAGE_SIZE = 10;
const int RECV_BUFFER_SIZE = IN_MESSAGE_SIZE + 2;

byte sendBuffer[SEND_BUFFER_SIZE];
char recvBuffer[RECV_BUFFER_SIZE];

typedef union{
	float floatPoint;
	byte bin[4];
} temp;

temp waterTemp, noodleTemp;

void setup(){
	pinMode(RFM95_RST, OUTPUT);
	digitalWrite(RFM95_RST, HIGH);

	Serial.begin(115200);
	delay(1000);

	pinMode(13, OUTPUT);
	waterTemp.floatPoint = 78.5743;
	noodleTemp.floatPoint = 52.3241;

	initLoRa();

	for(int i=0; i<SEND_BUFFER_SIZE; i++){
		sendBuffer[i] = 'N';
	}
}

void loop(){
	updateSendBuffer();
	// if(rf95.available()){
		sendToRX();
	// }

	// delay(500);

	// Serial.print(waterTemp.floatTemp);
	// Serial.print(": ");
	// for(int i=0; i<SEND_BUFFER_SIZE; i++){
	// 	Serial.print(sendBuffer[i]);
	// 	Serial.print(" ");
	// }
	// Serial.println();
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

void sendToRX(){
	// if(rf95.available()){
		digitalWrite(13, HIGH);

		// rf95.send((uint8_t *)sendBuffer, OUT_MESSAGE_SIZE);
		rf95.send((uint8_t *)sendBuffer, SEND_BUFFER_SIZE);
		rf95.waitPacketSent();
		// Serial.println("SENT!");
		digitalWrite(13, LOW);
	// }
}

void updateSendBuffer(){
	// b
	sendBuffer[0] = '/';
	for(int i=0; i<OUT_MESSAGE_SIZE/2; i++){
		sendBuffer[i+1] = waterTemp.bin[i];
		sendBuffer[i+5] = noodleTemp.bin[i];
	}
	sendBuffer[SEND_BUFFER_SIZE-1] = 0;
}