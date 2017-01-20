

// define interrupt pin


// define pinouts
// PAIR READ =====================
#define PIN_BLOWER_1_BTN		1      // air blower #1
#define PIN_BLOWER_2_BTN		2      // air blower #2
#define PIN_BLOWER_3_BTN		3      // air blower #3
#define PIN_HEATING_MBALL_BTN	4      // metal ball heating
#define PIN_RELEASE_MBALL_BTN	5      // metal ball release
#define PIN_EGG_BREAKER_BTN		6      // egg breaker
// PAIR OUT  - - - - - - - - - - -
#define PIN_BLOWER_1_OUT		7
#define PIN_BLOWER_2_OUT		8
#define PIN_BLOWER_3_OUT		9
#define PIN_HEATING_MBALL_OUT	9
#define PIN_RELEASE_MBALL_OUT	10
#define PIN_EGG_BREAKER_OUT		11

// NON PAIR READ =================
#define PIN_THERMAL_READ		12      // thermal analog read
#define PIN_NOODLE_UP			13
#define PIN_NOODLE_DOWN			14
// PAIR OUT  - - - - - - - - - - -
#define PIN_SERVO_DIR_OUT		15		// PWM
#define PIN_START_BUTTON 		16	
#define PIN_GPS_PPS         	0       // interrupt (NOT USED MAYBE)

#define NUM_OF_INPUTS  			9

#include <Adafruit_MAX31856.h>          // thermal module

// conflict check ... WORKS!
// ADXL accelerometer 
//#include <Adafruit_ADXL345_U.h>         // Adafruit Unified Sensor Driver !! 
										// https://github.com/adafruit/Adafruit_Sensor 

// use hardware SPI pin
Adafruit_MAX31856 thermal = Adafruit_MAX31856(PIN_THERMAL_READ);

enum actionState{ sNone, sP1, sBlower2, sBlower3, sHeatingBall, sReleaseBall, sEggBreak };
actionState action;

// state buffer
int 	readValue[16];
int 	lastReadValue[16];
bool	bReadUpdated[16];

// input pins
int inputPinGroup[NUM_OF_INPUTS]; 

void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);

	thermal.begin();
	thermal.setThermocoupleType(MAX31856_TCTYPE_K);

	initSys();
}

	void initSys(){
		initState();
		action = sNone;
		for(int i=0; i<NUM_OF_INPUTS; i++){
			intputPinGroup[i] = 0;
			// TODO : pinMode setting
			// ex) pinMode(inputPinGroup[i], INPUT);
		}

		

		// attachInterrupt
		attachInterrupt(PIN_GPS_PPS, pps_interrupt, RISING);
	}

		void initState(){
			for(int i=0; i<16; i++){
				readValue[i] = 0;
				lastReadValue[i] = 0;
				bReadUpdated[i] = false;
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

	update();
	action();
}



void pps_interrupt(){
	// TODO : synchronize with PPS from GPS breakout board
}

void update(){
	// check readPins and update state
	for(int i=0; i<16; i++){
		lastReadValue[i] = readValue[i];
		readValue[i] = gfeDigitalRead(i);

		if(lastReadValue[i] != readValue[i]){
			bReadUpdated[i] = true;
		} else	{
			bReadUpdated[i] = false;
		}

		if(bReadUpdate[i]){
			if(i == PIN_BLOWER_1_BTN)								action = sBlower1;
			else if(i == PIN_BLOWER_2_BTN)							action = sBlower2;
			else if(i == PIN_BLOWER_3_BTN)							action = sBlower3;
			else if(i == PIN_HEATING_MBALL_BTN)						action = sHeatingBall;
			else if(i == PIN_RELEASE_MBALL_BTN)						action = sReleaseBall;
			else if(i == PIN_EGG_BREAKER_BTN)						action = sEggBreak;
			else if((i == PIN_NOODLE_UP) || (i == PIN_NOODLE_DOWN))	action = sNoodleUpDown;
		}
		
	}
}

	
void action(actionState _state){
	switch(_state){
		case sBlower1 :
			// blower 1
			break;
		case sBlower2 :
			// blower 2 
			break;
		case sBlower3 :
			// blower 3
			break;
		case sHeatingBall:
			// heating ball
			break;
		case sReleaseBall:
			// release ball
			break;
		case sEggBeak:
			// break egg
			break;
		case sNoodleUpDown:
			// servo Control
			break;
		default:
			// read thermal degree
			// 
			break;
	}
}

// Pseudo function
bool gfeDigitalRead(int _pin){ 
	return digitalRead(_pin);	
}

int  gfeAnalogRead(int _pin){ 
	return analogRead(_pin);	
}

void gfeWrite(int _pin, bool _out) { 
	digitalWrite(_pin, _out); 
}

void gfeAnalogWrite(int _pin, int _out)	{ 
	analogWrite(_pin, _out);	
}
