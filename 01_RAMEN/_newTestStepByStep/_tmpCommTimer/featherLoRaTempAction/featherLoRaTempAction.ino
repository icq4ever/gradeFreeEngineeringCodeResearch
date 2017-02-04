#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_MAX31856.h>     // thermal module
#include <TimerOne.h>

const int PIN_LED 			= 13;

// === LoRa SPI / freq Setting ===================
const int RFM95_CS			= 8;
const int RFM95_RST			= 4;
const int RFM95_INT			= 7;
const float RF95_FREQ		= 433.0;
RH_RF95 rf95(RFM95_CS, RFM95_INT);

const int REQ_MESSAGE_SIZE = 2;
const int REQ_BUFFER_SIZE = REQ_MESSAGE_SIZE +2;

const int REPLY_MESSAGE_SIZE = 4;

const int RECV_MAX_BUFFER_SIZE = 20;
uint8_t recvBuffer[RECV_MAX_BUFFER_SIZE];
// ===============================================


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


void setup() {
	Serial.begin(115200);
	pinMode(PIN_LED, OUTPUT);

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
	// getTempData();
	// printTempData();
	receiveMessage();
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

		if(rf95.recv((char *)recvBuffer, &recvBufferLen)){
			// recv ok
			if(recvBuffer[0] == '/'){
				if(recvBuffer[1] == 'R'){
					digitalWrite(PIN_LED, HIGH);
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
				}
			}
		} else {
			// recv failed
		}
	}
}