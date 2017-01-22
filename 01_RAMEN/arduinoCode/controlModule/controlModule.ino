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
#define PIN_SOLENOID_1		A0		// 1, MSG
#define PIN_SOLENOID_2		A1		// 2, MSG
#define PIN_SOLENOID_3		A2		// 3, MSG
#define PIN_EGG_BREAKER		A3		// 4, MSG	
#define PIN_START_BTN		A4		// 5, P5	
#define PIN_HOT_WATER		A5		// 6, MSG	
#define PIN_DROP_MBALL		5		// 7, MSG	
#define PIN_HEATING_MBALL	6		// 8, MSG	
#define PIN_SERVO_UP		9		// 9
#define PIN_SERVO_DOWN		10		// 10

									//	  MSG : SERVO CTRL

#define PIN_LED 			13		// PIN_LED 	

#define PIN_SPI_SCK	 		SCK		//PIN_SPI_SCK	
#define PIN_SPI_MOSI 		MOSI	//PIN_SPI_MOSI
#define PIN_SPI_MISO		MISO	//PIN_SPI_MISO

#define PIN_RX				0		//PIN_RX		
#define PIN_TX				1		//PIN_TX		
#define PIN_SDA				2		//PIN_SDA		
#define PIN_SCL				3		//PIN_SCL		
// ==============================================================

// number of button message bit
#define NUM_OF_INPUT		10
#define NUM_OF_MESSAGE		8


// ==============================================================
// LoRa SETTING
#define RFM95_CS	8
#define RFM95_RST	4
#define RFM95_INT	7

// LoRa FREQ SETTING
#define RF95_FREQ			433.0

// sendBuffer
#define SEND_BUFFER_SIZE	51

// singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int 	inputPinList[NUM_OF_INPUT];
bool 	inputBtnStatus[NUM_OF_INPUT];
int 	actionMessage[NUM_OF_MESSAGE];
char	sendBuffer[SEND_BUFFER_SIZE];

void setup() {
	// start Serial vis USB
	Serial.begin(115200);

	inputPinList[0] = PIN_SOLENOID_1;
	inputPinList[1] = PIN_SOLENOID_2;
	inputPinList[2] = PIN_SOLENOID_3;
	inputPinList[3] = PIN_EGG_BREAKER;
	inputPinList[4] = PIN_HOT_WATER;
	inputPinList[5] = PIN_DROP_MBALL;
	inputPinList[6] = PIN_HEATING_MBALL;

	inputPinList[7] = PIN_START_BTN;
	inputPinList[8] = PIN_SERVO_UP;
	inputPinList[9] = PIN_SERVO_DOWN;

	// input Pin setup
	for(int i=0; i<NUM_OF_INPUT; i++){
		pinMode(inputPinList[i], INPUT);	
		inputBtnStatus[i] = 0;
	}

	// output pin setup
	pinMode(PIN_LED, OUTPUT);

	for(int i=0; i<NUM_OF_MESSAGE; i++){
		actionMessage[i] = 0;
	}

	while (!Serial);
	Serial.begin(115200);
	delay(100);

	initLoRa();
}

void loop() {
	updateBtnStatus();
	updateActionMessage();
	sendToActionBoard();
}

// sudo function definition
bool GFEDigitalRead(int _pin)					{ return digitalRead(_pin);	}
int  GFEAnalogRead(int _pin)					{ return analogRead(_pin);	}
void GFEDigitalWrite(int _pin, bool _out)		{ digitalWrite(_pin, _out); }
void GFEAnalogWrite(int _pin, int _out)			{ analogWrite(_pin, _out);	}

void initLoRa(){
	Serial.println("Feather LoRa RX Test!");

	manualLoRaReset();

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

int generateServoDirectionFlag(){
	// UP/DOWN all pressed or nothing pressed
	if(inputBtnStatus[inputPinList[8]] && inputBtnStatus[inputList[9]] || !inputBtnStatus[inputPinList[8]] && !inputBtnStatus[inputList[9]]){
		return 0;	// STOP
	} else {
		if(inputBtnStatus[inputPinList[8]])		return 1;	// up pressed : RIGHT
		else 									return -1;	// down pressed : LEFT
	}
}


void manualLoRaReset(){
	digitalWrite(RFM95_RST, LOW);
	delay(10);
	digitalWrite(RFM95_RST, HIGH);
	delay(10);
}

void updateBtnStatus(){
for(int i=0; i<NUM_OF_INPUT; i++){
		if(digitalRead(inputPinList[i]) !=0)	inputBtnStatus[i] = true;
		else									inputBtnStatus[i] = false;
	}
}

void updateActionMessage(){
	for(int i=0; i<NUM_OF_MESSAGE-1; i++){
		if(inputBtnStatus[i])	actionMessage[i] = 1;
		else					actionMessage[i] = 0;
	}

	inputBtnStatus[NUM_OF_MESSAGES-1] = generateServoDirectionFlag();
}

void sendToActionBoard(){
	digitalWrite(PIN_LED, HIGH);
	String tempStr = String("");
	tempStr+="M: ";
	for(int i=0; i<sizeof(actionMessage)-1; i++){
		tempStr+=actionMessage[i];
		tempStr+=", "; 
	}

	tempStr.toCharArray(sendBuffer, SEND_BUFFER_SIZE);

	rf95.send((uint8_t *)sendBuffer, tempStr.length());
	delay(10);
digitalWrite(PIN_LED, LOW);
}

void sendToP5(){
	// TODO : send to Processing
	// start?
	if(digitalRead(PIN_START_BTN)){
		Serial.println("S");
	}
}