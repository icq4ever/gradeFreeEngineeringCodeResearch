/* 
 *  RAMEN REMOTE CONTROL MODULE FIRMWARE
 *  by GradeFreeEngineering Team
 *  
 *  Song Hojun 
 *  Yi Donghoon
 * 
 * 
 *  TODO 
 *
 *  [ ] : COMMUNICATION TEST WITH PAIR OF feather LoRa 
 * 
 */


#include <SPI.h>
#include <RH_RF95.h>

// ==============================================================
// GFE STANDARD CONTROL PIN LIST
#define PIN_SOLENOID_1_BTN		A2		// 1, MSG
#define PIN_SOLENOID_2_BTN		A3		// 2, MSG
#define PIN_SOLENOID_3_BTN		A4		// 3, MSG
#define PIN_EGG_BREAKER_BTN		A5		// 4, MSG	
#define PIN_START_BTN			SCK		// 5, P5	
#define PIN_HOT_WATER_BTN		MOSI	// 6, MSG	
#define PIN_DROP_MBALL_BTN		MISO	// 7, MSG	
#define PIN_HEATING_MBALL_BTN	4		// 8, MSG	
#define PIN_SERVO_UP_BTN		5		// 9
#define PIN_SERVO_DOWN_BTN		9		// 10

#define PIN_LED 				13		// PIN_LED 	

// ==============================================================

// number of button message bit
#define NUM_OF_INPUT		10

#define OUT_MESSAGE_SIZE	9
#define SEND_BUFFER_SIZE	OUT_MESSAGE_SIZE+2

// IN_MESSAGE_SIZE / RECV_BUFFER_SIZE

// ==============================================================
// LoRa SETTING
#define RFM95_CS			8
#define RFM95_RST			4
#define RFM95_INT			7

// LoRa FREQ SETTING
#define RF95_FREQ			433.0

// sendBuffer
#define SEND_BUFFER_SIZE	51

// singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int 	inputPinList[NUM_OF_INPUT];
int 	inputBtnStatus[OUT_MESSAGE_SIZE];
char	sendBuffer[SEND_BUFFER_SIZE];

void setup() {
	// start Serial vis USB
	Serial.begin(115200);

	inputPinList[0] = PIN_SOLENOID_1_BTN;
	inputPinList[1] = PIN_SOLENOID_2_BTN;
	inputPinList[2] = PIN_SOLENOID_3_BTN;
	inputPinList[3] = PIN_EGG_BREAKER_BTN;
	inputPinList[4] = PIN_HOT_WATER_BTN;
	inputPinList[5] = PIN_HEATING_MBALL_BTN;
	inputPinList[6] = PIN_DROP_MBALL_BTN;
	inputPinList[7] = PIN_SERVO_UP_BTN;
	inputPinList[8] = PIN_SERVO_DOWN_BTN;
	
	inputPinList[9] = PIN_START_BTN;

	// input Pin setup
	for(int i=0; i<NUM_OF_INPUT; i++){
		pinMode(inputPinList[i], INPUT);	
		if(i != NUM_OF_INPUT-1)		inputBtnStatus[i] = 0;
	}

	// output pin setup
	pinMode(PIN_LED, OUTPUT);

	for(int i=0; i<OUT_MESSAGE_SIZE; i++){
		sendBuffer[i] = 'N';
	}

	while (!Serial);
	Serial.begin(115200);
	delay(100);

	initLoRa();
}

void loop() {
	updateBtnStatus();
	updateSendBuffer();
	sendToActionModule();
}

void initLoRa(){
	Serial.println("Feather LoRa TX Test!");

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

void updateBtnStatus(){
	for(int i=0; i<OUT_MESSAGE_SIZE; i++){
		if(digitalRead(inputPinList[i]) !=0)	inputBtnStatus[i] = true;
		else									inputBtnStatus[i] = false;
	}
}

void updateSendBuffer(){
	/*
		sendBuffer[] = { /, 1, 0, 1, 0, 1, 1, 1, 1...} 
	*/
	sendBuffer[0] = '/';
	for(int i=0; i<OUT_MESSAGE_SIZE; i++){
		if(inputBtnStatus[i])	sendBuffer[i+1] = '1';
		else					sendBuffer[i+1] = '0';
	}
	sendBuffer[SEND_BUFFER_SIZE-1] = 0;
}

void sendToActionModule(){
	digitalWrite(PIN_LED, HIGH);
	
	rf95.send((uint8_t *)sendBuffer, OUT_MESSAGE_SIZE);
	rf95.waitPacketSent();
	digitalWrite(PIN_LED, LOW);
}

void receiveFromActionModule(){
	// TODO : receive message from actionodule
	// waterTemp, noodleTemp, pps update signal
}

void sendToP5(){
	// TODO : send to Processing
	// start?
	// waterTemp, noodleTemp
	// pps update message
	if(digitalRead(PIN_START_BTN)){
		Serial.println("S");
	}
}