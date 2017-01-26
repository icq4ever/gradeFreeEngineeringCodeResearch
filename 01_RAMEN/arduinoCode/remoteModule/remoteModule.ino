/* 
 *  RAMEN REMOTE CONTROL MODULE FIRMWARE
 *  by GradeFreeEngineering Team
 *  
 *  Song Hojun 
 *  Yi Donghoon
 *  
 *  TODO : 
 *	[ ] : CC license attachment
 *  [ ] : thermal read and write To P5 via serial
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

#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_MAX31856.h>     // thermal module
#include <Servo.h>

// define pintouts
#define PIN_SOLENOID_1		A0
#define PIN_SOLENOID_2		A1 
#define PIN_SOLENIOD_3 		A2
#define PIN_EGG_BREAKER 	A3 
#define PIN_SIREN			A4

// thremal SPI pin
#define PIN_GPS_PPS			2	// as interrupt pin #2
#define PIN_HOT_WATER		3	// SCL

#define PIN_DROP_MBALL		5
#define PIN_HEATING_MBALL	6
#define PIN_SERVO_PWM		9
#define PIN_WATER_TEMP_CS	10
#define PIN_BUZZER_PWM		11
#define PIN_NOODLE_TEMP_CS	12
#define PIN_LED 			13

#define RECV_BUFFER_SIZE	9
#define IN_MESSAGE_SIZE		RECV_BUFFER_SIZE - 2

// LoRa SETTING
// LoRa SETTING
#define RFM95_CS			8
#define RFM95_RST			4
#define RFM95_INT			7

// LoRa FREQ 
#define RF95_FREQ			433.0	

#define SEND_BUFFER_SIZE	51

Adafruit_MAX31856 waterThermal = Adafruit_MAX31856(PIN_THERMAL_1_CS);
Adafruit_MAX31856 noodleThermal = Adafruit_MAX31856(PIN_THERMAL_2_CS);
RH_RF95 rf95(RFM95CS, RFM95_INT);
Servo servo;

uint8_t getBuffer[RECV_BUFFER_SIZE];
bool controlBtnStatus[IN_MESSAGE_SIZE];
int outputByPassPinList[NUM_OF_BYPASSOUT];


float waterTemp, noodleTemp;


void setup() {
	Serial.begin(115200);

	// init MAX31856 
	waterThermal.begin();
	noodleThermal.begin();
	waterThermal.setThermocoupleType(MAX31856_TCTYPE_K);
	noodleThermal.setThermocoupleType(MAX31956_TCTYPE_K);

	servo.sttach(PIN_SERVO_PWM_1);

	initLoRa();

	// attachInterrupt : GPS_PPS 
	attachInterrupt(GPS_PPS, pps_interrupt, RISING);

	outputBypassPinList[0] = PIN_SOLENOID_1;
	outputBypassPinList[1] = PIN_SOLENOID_2;
	outputBypassPinList[2] = PIN_SOLENOID_3;
	outputBypassPinList[3] = PIN_EGG_BREAKER;
	outputBypassPinList[4] = PIN_HOT_WATER;
	outputBypassPinList[5] = PIN_HEATING_MBALL;
	outputBypassPinList[6] = PIN_DROP_MBALL;
	
	// bypass Pinout set
	for(int i=0; i<NUM_OF_BYPASSOUT; i++){
		pinMode(outputByPassPinList[i], OUTPUT);
	}

	// PIN I/O setting
	pinMode(PIN_SIREN_1, OUTPUT);
	pinMode(PIN_SERVO_PWM_1, OUTPUT);		// PWM : noodle UP/DOWN
	pinMode(PIN_BUZZER_PWM, OUTPUT);		// PWM : BUZZER
	pinMode(PIN_LED, OUTPUT);
	

	// init messageFromActionMOdule
	for(int i=0; i<NUM_OF_IN_MESSAGE; i++){
		incommingMessage[i] = 0;
	}

	waterTemp = 0.f;
	noodleTemp = 0.f;
}

void loop() {
	// put your main code here, to run repeatedly:

	// print thermal temp
	// Serial.print(thermal.readThermocoupleTemperature());
	getTempData();	
	getMessage();
	action();
}

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
}

void getTempData(){
	// thermal ERROR HANDLING -> redefinition TODO
	uint8_t faultCheck_Water = waterThermal.readFault();
	uint8_t faultCheck_Noodle = noodleThermal.readFault();

	if (!faultCheck_Water) 		waterTemp = waterThermal.readThermocoupleTemperature();
	if (!faultCheck_Noodle)		noodleTemp = noodleThermal.readThermocoupleTemperature();
}

int generateServoDirectionFlag(){
	// UP/DOWN all pressed or nothing pressed
	if(inputBtnStatus[inputPinList[8]] && inputBtnStatus[inputList[9]] || !inputBtnStatus[inputPinList[8]] && !inputBtnStatus[inputList[9]]){
		return 2;	// STOP
	} else {
		if(inputBtnStatus[inputPinList[8]])		return 1;	// up pressed : RIGHT
		else 									return 3;	// down pressed : LEFT
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
	for(int i=0; i<NUM_OF_IN_MESSAGE-1; i++){
		if(incommingMessage[i] == 1)	digitalWrite(outputBypassPinList[i], HIGH);
		else 							digitalWRite(outputBypassPinList[i], LOW);
	}

	noodleUpDown(incommingMessage[NUM_OF_IN_MESSAGE-1]);
}


void receiveFromControlModule(){
	if(rf95.available()){
		uint8_t getBufferLen = sizeof(getBuffer);

		// PARSER 
		if(rf95.recv((char *)getBuffer, &getBufferLen)){
			digitalWrite(LED, HIGH);
			rf95.waitPacketSent();
			digitalWrite(LED, LOW);
		}
	}
}

void getControlBtnStatus(){
	if(getBuffer[0] == '/'){
		digitalWrite(PIN_LED, HIGH);
		for(int i=1; i<IN_MESSAGE_SIZE-1; i++){
			if((char)getBuffer[i] == '1')		controlBtnStatus[i-1] = true;
			else								controlBtnStatus[i-1] = false;
		}
		digitalWrite(PIN_LED, LOW);
	}
}

void sendToControlModule(){
	// TODO : send message to controlModule
	// waterTemp, noodleTemp, pps update signal
}