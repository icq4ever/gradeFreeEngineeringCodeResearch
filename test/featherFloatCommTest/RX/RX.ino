#include <SPI.h>
#include <RH_RF95.h>

const int RFM95_CS		= 8;
const int RFM95_RST		= 4;
const int RFM95_INT		= 7;

const int RF95_FREQ		= 433.0;

RH_RF95 rf95(RFM95_CS, RFM95_INT);

const int OUT_MESSAGE_SIZE 	= 11;
const int SEND_BUFFER_SIZE 	= OUT_MESSAGE_SIZE + 2;

const int IN_MESSAGE_SIZE	= 8;
const int RECV_BUFFER_SIZE	= IN_MESSAGE_SIZE + 2;

char 	sendBuffer[SEND_BUFFER_SIZE];
uint8_t recvBuffer[RECV_BUFFER_SIZE];

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

	waterTemp.floatPoint = 0.f;
	noodleTemp.floatPoint = 0.f;

	initLoRa();

	for(int i=0; i<SEND_BUFFER_SIZE; i++){
		sendBuffer[i] = 'N';
	}

	for(int i=0; i<RECV_BUFFER_SIZE; i++){
		recvBuffer[i] = 'N';
	}

}

void loop(){
	if(rf95.available()){
		receiveFromTX();
	}

	parseTemp();

	// Serial.print("water: ");
	// Serial.print(waterTemp.floatPoint, 4);
	// Serial.print("\t");
	// Serial.print(noodleTemp.floatPoint, 4);

	// for(int i=0; i<RECV_BUFFER_SIZE; i++){
	// 	Serial.print(recvBuffer[i]);
	// 	Serial.print(" ");
	// }
	Serial.println();
	// Serial.println(waterTemp.floatTemp);
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

void sendToTX(){
	digitalWrite(13, HIGH);

	rf95.send((uint8_t *)sendBuffer, OUT_MESSAGE_SIZE);
	rf95.waitPacketSent();
	Serial.println("SENT!");
	digitalWrite(13, LOW);
}

void receiveFromTX(){
	uint8_t recvBufferLen = sizeof(recvBuffer);

	if(rf95.recv((char *)recvBuffer, &recvBufferLen)){
		// nothing
		rf95.waitPacketSent();
	} else {
		// receive fail routine
		Serial.println("failed");
	}
}

void parseTemp(){
	if(recvBuffer[0] == '/'){
		// Serial.println("??");
		waterTemp.bin[0] = recvBuffer[1];
		waterTemp.bin[1] = recvBuffer[2];
		waterTemp.bin[2] = recvBuffer[3];
		waterTemp.bin[3] = recvBuffer[4];
		noodleTemp.bin[0] = recvBuffer[5];
		noodleTemp.bin[1] = recvBuffer[6];
		noodleTemp.bin[2] = recvBuffer[7];
		noodleTemp.bin[3] = recvBuffer[8];

		Serial.print(waterTemp.floatPoint, 4);
		Serial.print(" ");
		Serial.print(noodleTemp.floatPoint, 4);
		Serial.println();

	} else {
		

		// waterTemp = binTemp.floatTemp;
	}
}

// float bytesToFloat(uchar b0, uchar b1, uchar b2, uchar b3){
// 	binTemp.binary[3] = b0;
// 	binTemp.binary[2] = b1;
// 	binTemp.binary[1] = b2;
// 	binTemp.binary[0] = b3;

// 	return binTemp.floatTemp;


// 	// std::copy(reinterpret_cast<const char*>(&byte_array[0]),
// 	// 	reinterpret_cast<const char*>(&byte_array[4]),
// 	// 	reinterpret_casst<char*>(&result));

// 	// return result;
// }