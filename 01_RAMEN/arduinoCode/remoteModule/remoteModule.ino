/* 
 *  RAMEN REMOTE CONTROL MODULE FIRMWARE
 *  by GradeFreeEngineering Team
 *  
 *  Song Hojun 
 *  Yi Donghoon
 *  
 *  TODO : 
 *	[ ] : CC license attachment
 *  [ ] : thermal read and write To control Module via LoRa
 *  [V] : servo Direction Ctrl
 *  
 *  this code works with Adafruit feather basic module
 *  
 *  - 1 interrupt in for GPS PPS calibration
 *  - 1 D-in for thermal sensor Adafruit MAX31856 (HW SPI)
 *  - 3 D-out for solenoid blower
 *  - 1 D-out for siren control 
 *  - 1 D-out for hearing metal ball
 *  - 1 D-out for release metal ball
 *  - 1 D-out for eggbreaking
 *  - 1 A-out for control servo direction ( 0 : LEFT / 90 : STOP / 100 : RIGHT)
 *  
 */


// adafruit common sensor library download ->  https://github.com/adafruit/Adafruit_Sensor


#include <SPI.h>
#include <RH_RF95.h>				// RF module. 		library download -> 
#include <Adafruit_MAX31856.h>     	// thermal module 	library download -> https://github.com/adafruit/Adafruit_MAX31856
#include <Servo.h>

// define pintouts
const int PIN_SOLENOID_1		= A0;
const int PIN_SOLENOID_2		= A1;
const int PIN_SOLENOID_3 		= A2;
const int PIN_EGG_BREAKER 		= A3; 
const int PIN_SIREN				= A4;

// thremal SPI pin
const int PIN_GPS_PPS			= 2;	// as interrupt pin #2
const int PIN_HOT_WATER			= 3;	// SCL

const int PIN_DROP_MBALL		= 5;
const int PIN_HEATING_MBALL		= 6;
const int PIN_SERVO_PWM			= 9;
const int PIN_WATER_TEMP_CS		= 10;
const int PIN_BUZZER_PWM		= 11;
const int PIN_NOODLE_TEMP_CS	= 12;
const int PIN_LED 				= 13;


const int IN_MESSAGE_SIZE		= 9;
const int OUT_MESSAGE_SIZE		= 8;

const int RECV_BUFFER_SIZE		= IN_MESSAGE_SIZE+3;
const int SEND_BUFFER_SIZE		= OUT_MESSAGE_SIZE+2;


// LoRa SETTING
// LoRa SETTING
const int RFM95_CS				= 8;
const int RFM95_RST				= 4;
const int RFM95_INT				= 7;

// LoRa FREQ 
const float RF95_FREQ			= 433.0	;

const int NUM_OF_BYPASSOUT 		= 7;
const int BUZZER_MICROHERZ		= 250;
const int BUZZER_DUTYCYCLE		= 0.5;


Adafruit_MAX31856 waterThermal = Adafruit_MAX31856(PIN_WATER_TEMP_CS);
Adafruit_MAX31856 noodleThermal = Adafruit_MAX31856(PIN_NOODLE_TEMP_CS);


RH_RF95 rf95(RFM95_CS, RFM95_INT);
Servo servo;

uint8_t recvBuffer[RECV_BUFFER_SIZE];
char sendBuffer[SEND_BUFFER_SIZE];

bool controlBtnStatus[IN_MESSAGE_SIZE];
int outputBypassPinList[NUM_OF_BYPASSOUT];
bool bBuzzerHW;

typedef union{
	float floatPoint;
	byte bin[4];
} temp;

temp waterTemp, noodleTemp;

void setup() {
	pinMode(RFM95_RST, OUTPUT);
	digitalWrite(RFM95_RST, HIGH);
	
	Serial.begin(115200);
	delay(1000);

	bBuzzerHW = false;


	// init temps
	waterTemp.floatPoint = 0.f;
	noodleTemp.floatPoint = 0.f;

	// init MAX31856 
	waterThermal.begin();
	noodleThermal.begin();
	waterThermal.setThermocoupleType(MAX31856_TCTYPE_K);
	waterThermal.gfeSetAutoConvertMode();
	noodleThermal.setThermocoupleType(MAX31856_TCTYPE_K);
	noodleThermal.gfeSetAutoConvertMode();

	servo.attach(PIN_SERVO_PWM);

	initLoRa();


	// attachInterrupt : GPS_PPS 
	attachInterrupt(PIN_GPS_PPS, pps_interrupt, RISING);

	outputBypassPinList[0] = PIN_SOLENOID_1;
	outputBypassPinList[1] = PIN_SOLENOID_2;
	outputBypassPinList[2] = PIN_SOLENOID_3;
	outputBypassPinList[3] = PIN_EGG_BREAKER;
	outputBypassPinList[4] = PIN_HOT_WATER;
	outputBypassPinList[5] = PIN_HEATING_MBALL;
	outputBypassPinList[6] = PIN_DROP_MBALL;
	
	// bypass Pinout set
	for(int i=0; i<NUM_OF_BYPASSOUT; i++){
		pinMode(outputBypassPinList[i], OUTPUT);
	}

	// init sendBuffer
	for(int i=0; i<SEND_BUFFER_SIZE; i++){
		sendBuffer[i] = 'N';
	}

	// PIN I/O setting
	pinMode(PIN_SERVO_PWM, OUTPUT);		// PWM : noodle UP/DOWN
	pinMode(PIN_BUZZER_PWM, OUTPUT);		// PWM : BUZZER
	pinMode(PIN_LED, OUTPUT);
	

	// init messageFromActionModule
	for(int i=0; i<RECV_BUFFER_SIZE; i++){
		recvBuffer[i] = 'N';
	}
}

void loop() {
	// put your main code here, to run repeatedly:

	
	// updateSendBuffer();
	// sendToControlModule();

	receiveFromControlModule();	
	getControlBtnStatus();
	printButtonStatus();
	// checkButtonWithBuzzer();
	// delay(100);

	// b
}
	

void initLoRa(){
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

void pps_interrupt(){
	// TODO : synchronize with PPS from GPS breakout board
}

void siren(){
	// TODO :
	// trigger?
}

void buzzer(){
	// trigger ?
	// how ?
	// if (condition)		buzzerOn();
	// else					buzzerOff();
}



int generateServoDirectionFlag(){
	// UP/DOWN all pressed or nothing pressed
	if((controlBtnStatus[outputBypassPinList[9]] && controlBtnStatus[outputBypassPinList[10]]) || (!controlBtnStatus[outputBypassPinList[9]] && !controlBtnStatus[outputBypassPinList[10]])){
		return 2;	// STOP
	} else {
		if(controlBtnStatus[outputBypassPinList[8]])		return 1;	// up pressed : RIGHT
		else 												return 3;	// down pressed : LEFT
	}
}

// -1, 0, 1
// direct Servo control with PWM pin
void noodleUpDown(int _rotateCtrl){
	if(_rotateCtrl < 2)			servo.writeMicroseconds(2000);	// 1 : right 
	else if(_rotateCtrl > 2)	servo.writeMicroseconds(1000);	// 3 : left
	else						servo.writeMicroseconds(1500);	// 2 : stop
}

void action(){
	// for(int i=0; i<RECV_BUFFER_SIZE-1; i++){
	// 	if(recvBuffer[i] == 1)	digitalWrite(outputBypassPinList[i], HIGH);
	// 	else 					digitalWrite(outputBypassPinList[i], LOW);
	// }

	// noodleUpDown(recvBuffer[RECV_BUFFER_SIZE-1]);
}


void receiveFromControlModule(){
	if(rf95.available()){
		uint8_t recvBufferLen = sizeof(recvBuffer);

		// PARSER 
		if(rf95.recv((char *)recvBuffer, &recvBufferLen)){
			// digitalWrite(PIN_LED, HIGH);
			// rf95.waitPacketSent();
			// digitalWrite(PIN_LED, LOW);
		}
	}
}

void getControlBtnStatus(){
	if(recvBuffer[0] == '/' && (char)recvBuffer[1] == 'B'){
		for(int i=0; i<IN_MESSAGE_SIZE; i++){
			if(recvBuffer[i+2] == '1')		controlBtnStatus[i] = true;
			else							controlBtnStatus[i] = false;
		} 
	}

	if(recvBuffer[0] == '/' && (char)recvBuffer[1] == 'T'){
		getTempData();
		updateSendBuffer();
		sendToControlModule();
	}
}

void getTempData(){
	// thermal ERROR HANDLING -> redefinition TODO
	uint8_t faultCheck_Water = waterThermal.readFault();
	uint8_t faultCheck_Noodle = noodleThermal.readFault();

	if (!faultCheck_Water) 		waterTemp.floatPoint = waterThermal.gfeThermocoupleTemperature();
	if (!faultCheck_Noodle)		noodleTemp.floatPoint = noodleThermal.gfeThermocoupleTemperature();
}


void updateSendBuffer(){
	/*
		sendBuffer[] = { /, waterTemp.bin[0], waterTemp.bin[1] ... noodleTemp.bin[0], noodleTemp.bin[1]... } 
	*/
	sendBuffer[0] = '/';
	for(int i=0; i<OUT_MESSAGE_SIZE/2; i++){
		sendBuffer[i+1] = waterTemp.bin[i];
		sendBuffer[i+5] = noodleTemp.bin[i];
	}
	sendBuffer[SEND_BUFFER_SIZE-1] = 0;
}

void sendToControlModule(){
	// TODO : send message to controlModule
	// waterTemp, noodleTemp, pps update signal

	digitalWrite(PIN_LED, HIGH);

	rf95.send((uint8_t *) sendBuffer, SEND_BUFFER_SIZE);
	rf95.waitPacketSent();
	digitalWrite(PIN_LED, LOW);
}

void printTempData(){
	Serial.print("WaterTemp : ");	Serial.print(waterTemp.floatPoint, 4);
	Serial.print("\t");
	Serial.print("NoodleTemp: "); 	Serial.print(noodleTemp.floatPoint, 4);
	Serial.println();
}

void buzzerON(){
	// hz  4Khz
	// Duty Ratio : 1:1
	// duty cycle : 50%
	if((micros()%BUZZER_MICROHERZ*BUZZER_DUTYCYCLE) < BUZZER_MICROHERZ*BUZZER_DUTYCYCLE)	bBuzzerHW = true;
	else																					bBuzzerHW = false;
	analogWrite(A4, bBuzzerHW);
}

void printButtonStatus(){
	for(int i=0; i<RECV_BUFFER_SIZE; i++){
		Serial.print((char)recvBuffer[i]);
		Serial.print(", ");
	}
	Serial.println();

	// for(int i=0; i<IN_MESSAGE_SIZE; i++){
	// 	if(controlBtnStatus[i])	{
	// 		Serial.print('1');
	// 	} else {
	// 		Serial.print('0');
	// 	}
	// 	Serial.print(", ");
	// }
	// Serial.println();
}