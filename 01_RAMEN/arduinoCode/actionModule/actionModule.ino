#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_MAX31856.h>     // thermal module



// === LoRa SPI / freq Setting ===================
const int RFM95_CS			= 8;
const int RFM95_RST			= 4;
const int RFM95_INT			= 7;
const float RF95_FREQ		= 433.0;

RH_RF95 rf95(RFM95_CS, RFM95_INT);
Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver();
const int SERVOMIN = 1000;
const int SERVOMAX = 2000;
uint8_t servoNum = 7;

const int REQ_MESSAGE_SIZE = 2;
const int REQ_BUFFER_SIZE = REQ_MESSAGE_SIZE +2;

const int REPLY_MESSAGE_SIZE = 4;

const int RECV_MAX_BUFFER_SIZE = 20;
uint8_t recvBuffer[RECV_MAX_BUFFER_SIZE];
bool lastServoStop, lastServoUp, lastServoDown;
// ===============================================

/*

MESSAGE INFO
[0] : PIN_SOLENOID_1_BTNå
[1] : PIN_SOLENOID_2_BTN
[2] : PIN_SOLENOID_3_BTN
[3] : PIN_DROP_MBALL_BTN
[4] : PIN_HEATING_MBALL_BTN
[5] : PIN_SERVO_UP_BTN
[6] : PIN_SERVO_DOWN_BTN
[7] : PIN_START_BTN
[8] : PIN_EGG_BREAKER_BTN
[9] : PIN_HOT_WATER



*/

// define pintouts
const int PIN_SOLENOID_1		= A0;
const int PIN_SOLENOID_2		= A1;
const int PIN_SOLENOID_3 		= A2;
const int PIN_DROP_MBALL		= 5;
const int PIN_HEATING_MBALL		= 6;
const int PIN_EGG_BREAKER 		= A3; 
const int PIN_HOT_WATER			= A5;	// SCL

const int PIN_SERVO_PWM			= 9;
const int PIN_SIREN				= A4;
const int PIN_BUZZER_PWM		= 11 ;	
const int PIN_LED 				= 13;

// thremal SPI pin
const int PIN_GPS_PPS			= 2;	// as interrupt pin #2



// === MAX31856 Thermalcoupler setting ===========
const int PIN_WATER_TEMP_CS	 = 10;
const int PIN_NOODLE_TEMP_CS = 12;

Adafruit_MAX31856 waterThermal = Adafruit_MAX31856(PIN_WATER_TEMP_CS);
Adafruit_MAX31856 noodleThermal = Adafruit_MAX31856(PIN_NOODLE_TEMP_CS);

typedef union{
	float tempFloat;
	byte tempBin[4];	// union : float / 4 binary
} temperature;

temperature waterTemp, noodleTemp;
// ===============================================

bool btnStatus[10];

const int NUM_OF_BYPASSOUT = 7;
int outputBypassPinList[NUM_OF_BYPASSOUT];

const int BUZZER_MICROHERZ		= 250;
const int BUZZER_DUTYCYCLE		= 0.5;
// Servo servo;

enum btnTypes{
	BTN_MOMENT,
	BTN_SHORT_PRESS,
	BTN_LONG_PRESS
};

/*

btnStatus[0] : SOLENOID_1
btnStatus[1] : SOLENOID_2
btnStatus[2] : SOLENOID_3
btnStatus[3] : DROP_MBALL
btnStatus[4] : HEATING_MBALL
btnStatus[5] : SERVO_UP
btnStatus[6] : SERVO_DOWN
btnStatus[7] : START_BTN
btnStatus[8] : EGG_BREAKER
btnStatus[9] : HOT_WATER

*/

btnTypes btnTypeTable[10]={
	BTN_MOMENT,
	BTN_MOMENT,
	BTN_MOMENT,
	BTN_SHORT_PRESS,
	BTN_SHORT_PRESS,
	BTN_MOMENT,
	BTN_MOMENT,
	BTN_LONG_PRESS,
	BTN_SHORT_PRESS,
	BTN_SHORT_PRESS
};

void setup() {
	Serial.begin(115200);

	outputBypassPinList[0] = PIN_SOLENOID_1;
	outputBypassPinList[1] = PIN_SOLENOID_2;
	outputBypassPinList[2] = PIN_SOLENOID_3;
	outputBypassPinList[3] = PIN_DROP_MBALL;
	outputBypassPinList[4] = PIN_HEATING_MBALL;
	outputBypassPinList[5] = PIN_EGG_BREAKER;
	outputBypassPinList[6] = PIN_HOT_WATER;
	
	for(int i=0; i<NUM_OF_BYPASSOUT; i++){
		pinMode(outputBypassPinList[i], OUTPUT);
	}

	pinMode(PIN_SERVO_PWM, OUTPUT);
	pinMode(PIN_SIREN, OUTPUT);
	// pinMode(PIN_BUZZER_PWM, OUTPUT);
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, LOW);

	lastServoUp = lastServoDown = lastServoUp = false;

	servo.begin();
	servo.setPWMFreq(50);
	// servo.attach(PIN_SERVO_PWM, 1000, 2000);
	// servo.writeMicroseconds(1500);


	// init MAX31856 
	waterThermal.begin();
	waterThermal.setThermocoupleType(MAX31856_TCTYPE_K);
	noodleThermal.begin();
	noodleThermal.setThermocoupleType(MAX31856_TCTYPE_K);

	delay(1000);

	

	waterTemp.tempFloat = 0.f;
	noodleTemp.tempFloat = 0.f;

	for(int i=0; i<10; i++){
		btnStatus[i] = false;
	}

	initLoRa();

}

// void setServoPulse(uint8_t n, double pulse) {
//   double pulselength;
  
//   pulselength = 1000000;   // 1,000,000 us per second
//   pulselength /= 60;   // 60 Hz
//   Serial.print(pulselength); Serial.println(" us per period"); 
//   pulselength /= 4096;  // 12 bits of resolution
//   Serial.print(pulselength); Serial.println(" us per bit"); 
//   pulse *= 1000;
//   pulse /= pulselength;
//   Serial.println(pulse);
//   servo.setPWM(n, 0, pulse);
// }

void loop() {
	// getTempData();
	// printTempData();
	receiveMessage();
	action();	
	// printRecvBuffer();
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

void getTempData(){
	uint8_t faultCheckWater = waterThermal.readFault();
	uint8_t faultCheckNoodle = noodleThermal.readFault();

	if (!faultCheckWater) 	waterTemp.tempFloat = waterThermal.gfeThermocoupleTemperature();
	if (!faultCheckNoodle)	noodleTemp.tempFloat = noodleThermal.gfeThermocoupleTemperature();
}

void printTempData(){
	Serial.print(waterTemp.tempFloat, 4);
	Serial.print(", ");
	Serial.print(noodleTemp.tempFloat, 4);
	Serial.println();
}

void receiveMessage(){
	if(rf95.available()){
		uint8_t recvBufferLen = sizeof(recvBuffer);

		if(rf95.recv(recvBuffer, &recvBufferLen)){
			digitalWrite(PIN_LED, HIGH);
			// recv ok
			if(recvBuffer[0] == '/'){
				if(recvBuffer[1] == 'R'){
					// digitalWrite(PIN_LED, HIGH);
					getTempData();

					uint8_t reply[10];
					reply[0] = '/';
					reply[1] = waterTemp.tempBin[0];
					reply[2] = waterTemp.tempBin[1];
					reply[3] = waterTemp.tempBin[2];
					reply[4] = waterTemp.tempBin[3];
					reply[5] = noodleTemp.tempBin[0];
					reply[6] = noodleTemp.tempBin[1];
					reply[7] = noodleTemp.tempBin[2];
					reply[8] = noodleTemp.tempBin[3];
					reply[9] = 0;

					rf95.send(reply, sizeof(reply));
					digitalWrite(PIN_LED, LOW);
					Serial.println("Temp Data Sent!");
					printTempData();
				}

				if(recvBuffer[1] == 'B'){
					// digitalWrite(PIN_LED, HIGH);

					// parse message 
					for(int i=0; i<sizeof(btnStatus); i++){
						if(recvBuffer[i+2] == '1')	btnStatus[i] = true;
						else						btnStatus[i] = false;
					}

					// printBtnStatus();
					// printRecvBuffer();
				}
			}
			digitalWrite(PIN_LED, LOW);
		} else {
			// recv failed
		}
	}
}

void action(){
	/*

	btnStatus[0] : SOLENOID_1
	btnStatus[1] : SOLENOID_2
	btnStatus[2] : SOLENOID_3
	btnStatus[3] : DROP_MBALL
	btnStatus[4] : HEATING_MBALL
	btnStatus[5] : SERVO_UP
	btnStatus[6] : SERVO_DOWN
	btnStatus[7] : START_BTN
	btnStatus[8] : EGG_BREAKER
	btnStatus[9] : HOT_WATER

	*/
	if(btnStatus[0])	{
		digitalWrite(PIN_SOLENOID_1, HIGH);
		Serial.println("SOLENOID 1 !!");
	}
	else				digitalWrite(PIN_SOLENOID_1, LOW);

	if(btnStatus[1]){	
		digitalWrite(PIN_SOLENOID_2, HIGH);
		Serial.println("SOLENOID 2 !!");
	}
	else				digitalWrite(PIN_SOLENOID_2, LOW);

	if(btnStatus[2])	{
		digitalWrite(PIN_SOLENOID_3, HIGH);
		Serial.println("SOLENOID 3 !!");
	}
	else				digitalWrite(PIN_SOLENOID_3, LOW);

	if(btnStatus[3])	{
		digitalWrite(PIN_DROP_MBALL, HIGH);
		Serial.println("BALL DROP !!");
	}
	else				digitalWrite(PIN_DROP_MBALL, LOW);	

	if(btnStatus[4])	{
		digitalWrite(PIN_HEATING_MBALL, HIGH);
		Serial.println("BALL HEAT !!");
	}
	else				digitalWrite(PIN_HEATING_MBALL, LOW);

	if(btnStatus[8])	{
		digitalWrite(PIN_EGG_BREAKER, HIGH);
		Serial.println("EGG BREAK !!");
	}
	else				digitalWrite(PIN_EGG_BREAKER, LOW);		

	if(btnStatus[9])	{
		digitalWrite(PIN_HOT_WATER, HIGH);
		Serial.println("HOT WATER !!");
	}
	else				digitalWrite(PIN_HOT_WATER, LOW);

	if(btnStatus[7]){
		Serial.println("START BTN !!!");
	}

	// servo control (pin 5, 6)
	noodleUpDown(generateServoDirectionFlag());

}

int generateServoDirectionFlag(){
	// UP/DOWN all pressed or nothing pressed
	if( btnStatus[5] == false && btnStatus[6] == false ){
		return 2;	// STOP
	} 

	if(btnStatus[5] == true && btnStatus[6] == false)		return 1;	// up pressed : RIGHT
	if(btnStatus[6] == true && btnStatus[5] == false)	 	return 3;	// down pressed : LEFT
}

void noodleUpDown(int _rotateCtrl){

	if(_rotateCtrl < 2)	{
		if(!lastServoUp){
			// Serial.println("Up");
			servo.setPWM(0, 0, 409);	// 1 : right 
			lastServoStop = false;
			lastServoUp = true;
			lastServoDown = false;
		}
	} else if(_rotateCtrl > 2)	{
		if(!lastServoDown){
			// Serial.println("Down");
			servo.setPWM(0, 0, 205);	// 3 : left
			lastServoStop = false;
			lastServoUp = false;
			lastServoDown = true;
		}
	} else	{
		if(!lastServoStop){
			Serial.println("stop");
			servo.setPWM(0, 0, 290);	// 2 : stop
			lastServoStop = true;
			lastServoUp = false;
			lastServoDown = false;
		}
	}	
}

void printBtnStatus(){
	for(int i=0; i<sizeof(btnStatus); i++){
		if(btnStatus[i])	Serial.print('1');
		else				Serial.print('0');
		Serial.print(',');
	}
	Serial.println();
}

void printRecvBuffer(){
	for(int i=0; i<RECV_MAX_BUFFER_SIZE; i++){
		Serial.print((char)recvBuffer[i]);
		Serial.print(",");
	}
	Serial.println();
}

