// define interrupt pin
#define PIN_GPS_PPS         0       // gps pps pulse read

// define output pinouts
#define PIN_BLOWER_1        11      // air blower #1
#define PIN_BLOWER_2        12      // air blower #2
#define PIN_BLOWER_3        13      // air blower #3
#define PIN_THERMAL_READ    14      // thermal read
#define PIN_BALL_HEATING    15      // metal ball heating
#define PIN_BALL_RELEASE    16      // metal ball release
#define PIN_EGG_BREAKER     17      // egg breaker

#include <Adafruit_MAX31856.h>     // thermal module

// use hardware SPI pin
Adafruit_MAX31856 thermal = Adafruit_MAX31856(THERMAL_READ);

void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);

	thermal.begin();
	thermal.setThermocoupleType(MAX31856_TCTYPE_K);

	// attachInterrupt
	attachInterrupt(GPS_PPS, pps_interrupt, RISING);

	pinMode(PIN_BLOWER_1, OUTPUT);
	pinMode(PIN_BLOWER_2, OUTPUT);
	pinMode(PIN_BLOWER_3, OUTPUT);
	pinMode(PIN_BALL_HEATING, OUTPUT);
	pinMode(PIN_BALL_RELEASE, OUTPUT);
	pinMode(PIN_EGG_BREAKER, OUTPUT);
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
}

void pps_interrupt(){
	// TODO : synchronize with PPS from GPS breakout board
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