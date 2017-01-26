/* 
 *  RAMEN REMOTE CONTROL MODULE FIRMWARE
 *  by GradeFreeEngineering Team
 *  
 *  Song Hojun 
 *  Yi Donghoon
 *  
 *  TODO : 
 *	[ ] : CC license attachment
 *  [V] : thermal read Check
 *  [V] : servo Direction Ctrl
 *  [ ] : gps pps pulse read
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


#define PIN_WATER_TEMP_CS	10
#define PIN_LED 			13


// LoRa SETTING
// LoRa SETTING
#define RFM95_CS			8
#define RFM95_RST			4
#define RFM95_INT			7

// LoRa FREQ 
#define RF95_FREQ			433.0	

Adafruit_MAX31856 waterThermal = Adafruit_MAX31856(PIN_WATER_TEMP_CS);

RH_RF95 rf95(RFM95_CS, RFM95_INT);

float waterTemp;


void setup() {
	Serial.begin(115200);

	// init MAX31856 
	waterThermal.begin();
	waterThermal.setThermocoupleType(MAX31856_TCTYPE_K);

	initLoRa();

	waterTemp = 0.f;
}

void loop() {
	// put your main code here, to run repeatedly:

	// print thermal temp
	// Serial.print(thermal.readThermocoupleTemperature());

	getTempData();
	Serial.println(waterTemp);
	delay(100);
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
}

void getTempData(){
	// thermal ERROR HANDLING -> redefinition TODO
	uint8_t fault = waterThermal.readFault();
	if (!fault) {
		// get temp 
		waterTemp = waterThermal.readThermocoupleTemperature();
	}
}
