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

// define pintouts
#define PIN_SOLENOID_1		A0
#define PIN_SOLENOID_2		A1 
#define PIN_SOLENIOD_3 		A2
#define PIN_EGG_BREAKER 	A3 
#define PIN_SIREN_1			A4
#define PIN_SIREN_2			A5

// thremal SPI pin
#define PIN_THERMAL_SCK		SCK
#define PIN_THERMAL_MOSI 	MOSI
#define PIN_THERMAL_MISO	MISO 

#define PIN_GPS_RX			0
#define PIN_GPS_TX			1

#define PIN_GPS_PPS			2	// as interrupt pin #2
#define PIN_HOT_WATER		3

#define PIN_DROP_MBALL		5
#define PIN_HOT_MBALL		6
#define PIN_SERVO_PWM_1		9
#define PIN_SERVO_PWM_2		10
#define PIN_SIREN_PWM		11
#define PIN_SERVO_PWM_3		12
#define PIN_LED 			13


#include <Adafruit_MAX31856.h>     // thermal module

bool blower1ON = false;
bool blower2ON = false;
bool blower3ON = false;
bool ballHeating = false;
bool ballRelease = false;
bool eggBreak = false;

int messageFromP5[7];
// use hardware SPI pin
Adafruit_MAX31856 thermal = Adafruit_MAX31856(THERMAL_READ);

void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);

	thermal.begin();
	thermal.setThermocoupleType(MAX31856_TCTYPE_K);

	// attachInterrupt
	attachInterrupt(GPS_PPS, pps_interrupt, RISING);


	// we have 6 controlPin.
	pinMode(PIN_BLOWER_1, OUTPUT);
	pinMode(PIN_BLOWER_2, OUTPUT);
	pinMode(PIN_BLOWER_3, OUTPUT);
	pinMode(PIN_BALL_HEATING, OUTPUT);
	pinMode(PIN_BALL_RELEASE, OUTPUT);
	pinMode(PIN_EGG_BREAKER, OUTPUT);
	pinMode(PIN_SERVO, OUTPUT);


	// init messageFromP5 Buffer
	for(int i=0; i<sizeof(messageFromP5)/sizeof(messageFromP5[0]); i++){
		messageFromP5[i] = 0;
	}
}

void loop() {
	// put your main code here, to run repeatedly:

	// print thermal temp
	Serial.print(thermal.readThermocoupleTemperature());

	// thermal ERROR HANDLING -> redefinition TODO
	uint8_t fault = thermal.readFault();
	if (fault) {
		if (fault & MAX31856_FAULT_CJRANGE) Serial.println("Cold Junction Range Fault");
		if (fault & MAX31856_FAULT_TCRANGE) Serial.println("Thermocouple Range Fault");
		if (fault & MAX31856_FAULT_CJHIGH)  Serial.println("Cold Junction High Fault");
		if (fault & MAX31856_FAULT_CJLOW)   Serial.println("Cold Junction Low Fault");
		if (fault & MAX31856_FAULT_TCHIGH)  Serial.println("Thermocouple High Fault");
		if (fault & MAX31856_FAULT_TCLOW)   Serial.println("Thermocouple Low Fault");
		if (fault & MAX31856_FAULT_OVUV)    Serial.println("Over/Under Voltage Fault");
		if (fault & MAX31856_FAULT_OPEN)    Serial.println("Thermocouple Open Fault");
	}

	if(messageFromP5[0])	digitalWrite(PIN_BLOWER_1, HIGH);
	else					digitalWrite(PIN_BLOWER_1, LOW);

	if(messageFromP5[1])	digitalWrite(PIN_BLOWER_2, HIGH);
	else					digitalWrite(PIN_BLOWER_2, LOW);

	if(messageFromP5[2])	digitalWrite(PIN_BLOWER_3, HIGH);
	else					digitalWrite(PIN_BLOWER_3, LOW);

	if(messageFromP5[4])	digitalWrite(PIN_BALL_HEATING, HIGH);
	else					digitalWrite(PIN_BALL_HEATING, LOW);

	if(messageFromP5[5])	digitalWrite(PIN_BALL_RELEASE, HIGH);
	else					digitalWrite(PIN_BALL_RELEASE, LOW);

	noodleUpDown(messageFromP5[6]);

}

void pps_interrupt(){
	// TODO : synchronize with PPS from GPS breakout board
}

// -1, 0, 1
// direct Servo control with PWM pin
void noodleUpDown(int _rotateCtrl){
	if(_rotateCtrl < 0)			analogWrite(PIN_SERVO, 0);		// left
	else if(_rotateCtrl > 0)	analogWrite(PIN_SERVO, 100);	// right 
	else						analogWrite(PIN_SERVO, 90);		// stop
}


// sudo function definition
bool GFEDigitalRead(int _pin){ 
	return digitalRead(_pin);	
}

int  GFEAnalogRead(int _pin){ 
	return analogRead(_pin);	
}

void GFEDigitalWrite(int _pin, bool _out) { 
	digitalWrite(_pin, _out); 
}

void GFEAnalogWrite(int _pin, int _out)	{ 
	analogWrite(_pin, _out);	
}


void serialEvent(){
	// TODO : send thermal degree to processing
	
}