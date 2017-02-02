#include <SPI.h>
#include <RH_RF95.h>

const int PIN_LED 		= 13;

const int RFM95_CS		= 8;
const int RFM95_RST		= 4;
const int RFM95_INT		= 7;

const float RF95_FREQ	= 433.0;

// == Thermalcoupler buffer, data setup ==========
const int RECV_MESSAGE_SIZE = 4;
const int RECV_BUFFER_SIZE = RECV_MESSAGE_SIZE + 3;
uint8_t recvBuffer[RECV_BUFFER_SIZE];

typedef union{
	float tempFloat;
	byte tempBin[4];	// union : float / 4 binary
} temperature;

temperature waterTemp, noodleTemp;
// ===============================================

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup(){
	Serial.begin(115200);

	delay(1000);
}

void loop(){
	// delay(1000);
	initLoRa();
}

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

void requestTempToActionmodule(){
	char requestPacket[3] = "/R";
	requestPacket[2] = 0;

	rf95.send((uint8_t *) requestPacket, 3);

	rf95.waitPacketSent();
	receiveTempFromActionModule();
}

void receiveTempFromActionModule(){
	if(rf95.waitAvailableTimeout(500)){
		uint8_t recvBufferLen = sizeof(recvBuffer);

		if(rf95.recv((char *)recvBuffer, &recvBufferLen))	{	Serial.println("Received OK!");		}
		else												{	Serial.println("Receive Failed");	}

		if(recvBuffer[0] == '/'){
			waterTemp.tempBin[0] = recvBuffer[1];
			waterTemp.tempBin[1] = recvBuffer[2];
			waterTemp.tempBin[2] = recvBuffer[3];
			waterTemp.tempBin[3] = recvBuffer[4];

			noodleTemp.tempBin[0] = recvBuffer[5];
			noodleTemp.tempBin[1] = recvBuffer[6];
			noodleTemp.tempBin[2] = recvBuffer[7];
			noodleTemp.tempBin[3] = recvBuffer[8];
		}
	} 
	else {
		Serial.println("Action Module Not reply.........");
	}
}