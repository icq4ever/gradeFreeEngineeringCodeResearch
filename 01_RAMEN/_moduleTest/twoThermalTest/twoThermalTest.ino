/* 
 *  RAMEN REMOTE CONTROL MODULE FIRMWARE
 *  by GradeFreeEngineering Team
 *  
 *  
  two max31856 module test
 *  
 */


// adafruit common sensor library download ->  https://github.com/adafruit/Adafruit_Sensor


#include <SPI.h>
#include <RH_RF95.h>				// RF module. 		library download -> 
#include <Adafruit_MAX31856.h>     	// thermal module 	library download -> https://github.com/adafruit/Adafruit_MAX31856

// define pintouts
#define PIN_WATER_TEMP_CS	10
#define PIN_NOODLE_TEMP_CS	12
#define PIN_LED 			13

// LoRa SETTING
// LoRa SETTING
#define RFM95_CS			8
#define RFM95_RST			4
#define RFM95_INT			7

// LoRa FREQ 
#define RF95_FREQ			433.0	

#define IN_MESSAGE_SIZE		4
#define OUT_MESSAGE_SIZE	10	// button status

#define RECV_BUFFER_SIZE	IN_MESSAGE_SIZE+2
#define SEND_BUFFER_SIZE	OUT_MESSAGE_SIZE+2


Adafruit_MAX31856 waterThermal = Adafruit_MAX31856(PIN_WATER_TEMP_CS);
Adafruit_MAX31856 noodleThermal = Adafruit_MAX31856(PIN_NOODLE_TEMP_CS);

// singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

float waterTemp, noodleTemp;


void setup() {
	Serial.begin(115200);

	// init MAX31856 
	waterThermal.begin();
	noodleThermal.begin();
	waterThermal.setThermocoupleType(MAX31856_TCTYPE_K);
	noodleThermal.setThermocoupleType(MAX31856_TCTYPE_K);

	initLoRa();
	delay(1000);

	// PIN I/O setting
	
	waterTemp = 0.f;
	noodleTemp = 0.f;
}

void loop() {
	// put your main code here, to run repeatedly:

	// print thermal temp
	// Serial.print(thermal.readThermocoupleTemperature());
	getTempData();	
	// getMessage();
	// action();


	Serial.print("WaterTemp : ");	Serial.print(waterTemp, 4);
	Serial.print("\t");
	Serial.print("NoodleTemp: "); 	Serial.print(noodleTemp, 4);
	Serial.println();
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


void getTempData(){
	// thermal ERROR HANDLING -> redefinition TODO
	uint8_t faultCheck_Water = waterThermal.readFault();
	if (!faultCheck_Water){
		// Serial.println("water Read!");
		waterTemp = waterThermal.readThermocoupleTemperature();
	} 


	uint8_t faultCheck_Noodle = noodleThermal.readFault();
	if (!faultCheck_Noodle)		{
		// Serial.println("noodle Read!");
		noodleTemp = noodleThermal.readThermocoupleTemperature();
	}
}
