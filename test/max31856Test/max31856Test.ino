#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_MAX31856.h>     // thermal module

const int PIN_LED 			= 13;

const int RFM95_CS			= 8;
const int RFM95_RST			= 4;
const int RFM95_INT			= 7;

const int PIN_WATER_TEMP_CS	 = 10;
const int PIN_NOODLE_TEMP_CS = 12;

// LoRa FREQ 
const float RF95_FREQ		= 433.0;

Adafruit_MAX31856 waterThermal = Adafruit_MAX31856(PIN_WATER_TEMP_CS);
Adafruit_MAX31856 noodleThermal = Adafruit_MAX31856(PIN_NOODLE_TEMP_CS);

RH_RF95 rf95(RFM95_CS, RFM95_INT);

typedef union{
	float tempFloat;
	byte tempBin[4];
} temperature;

temperature waterTemp, noodleTemp;

void setup() {
	Serial.begin(115200);

	// init MAX31856 
	waterThermal.begin();
	waterThermal.setThermocoupleType(MAX31856_TCTYPE_K);
	noodleThermal.begin();
	noodleThermal.setThermocoupleType(MAX31856_TCTYPE_K);

	delay(1000);

	initLoRa();

	waterTemp.tempFloat = 0.f;
	noodleTemp.tempFloat = 0.f;

}

void loop() {
	// put your main code here, to run repeatedly:

	// print thermal temp
	// Serial.print(thermal.readThermocoupleTemperature());

	getTempData();
	printTempData();
}

void initLoRa() { // init
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
	// thermal ERROR HANDLING -> redefinition TODO
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
