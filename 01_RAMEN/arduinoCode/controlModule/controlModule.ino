/* 
 *  RAMEN REMOTE CONTROL MODULE FIRMWARE
 *  by GradeFreeEngineering Team
 *  
 *  Song Hojun 
 *  Yi Donghoon
 *  
 */

// GFE STANDARD CONTROL PIN LIST
#define PIN_SOLENOID_1		A0		// PIN_BTN_1	
#define PIN_SOLENOID_2		A1		// PIN_BTN_2	
#define PIN_SOLENOID_3		A2		// PIN_BTN_3	
#define PIN_EGG_BREAKER		A3		// PIN_BTN_4	
#define PIN_SIREN_1			A4		// PIN_BTN_5	
#define PIN_SIREN_2			A5		// PIN_BTN_6	
#define PIN_HOT_WATER		5		// PIN_BTN_7	
#define PIN_DROP_MBALL		6		// PIN_BTN_8	
#define PIN_SERVO_UP		9
#define PIN_SERVO_DOWN		10		

#define PIN_START_BUTTON	11		// PIN_PWM_1	
#define PIN_PWM_2			12		// PIN_PWM_2	
#define PIN_LED 			13		// PIN_LED 	

#define PIN_SPI_SCK	 		SCK		//PIN_SPI_SCK	
#define PIN_SPI_MOSI 		MOSI	//PIN_SPI_MOSI
#define PIN_SPI_MISO		MISO	//PIN_SPI_MISO

#define PIN_RX				0		//PIN_RX		
#define PIN_TX				1		//PIN_TX		
#define PIN_SDA				2		//PIN_SDA		
#define PIN_SCL				3		//PIN_SCL		

// number of button message bit
#define NUM_OF_OUTPUT		10

int inputBufferPin[NUM_OF_OUTPUT];		// store pins
int messageBuffer[NUM_OF_OUTPUT];		// store input of pin

void setup() {
	// start Serial vis USB
	Serial.begin(115200);

	inputBufferPin[0]	= PIN_SOLENOID_1;	
	inputBufferPin[1]	= PIN_SOLENOID_2;	
	inputBufferPin[2]	= PIN_SOLENOID_3;
	inputBufferPin[3]	= PIN_HEATING_MBALL;
	inputBufferPin[4]	= PIN_DROP_MBALL;
	inputBufferPin[5]	= PIN_EGG_BREAKER;
	inputBufferPin[6]	= PIN_SERVO_UP;
	inputBufferPin[7]	= PIN_SERVO_DOWN;
	inputBufferPin[8]	= PIN_SIREN_1;
	inputBufferPin[9]	= PIN_SIREN_2;


	pinMode(PIN_LED, OUTPUT);

	for(int i=0; i<9; i++){
		messageBuffer[i] = 0;
		pinMode(inputBufferPin[i], INPUT);
	}
}

void loop() {

	for(int i=0; i<NUM_OF_OUTPUT; i++){
		if(GFEDigitalRead(inputBufferPin[i]))	messageBuffer[i] = 1;
		else									messageBuffer[i] = 0;
	}

	sendToActionBoard();
}

// sudo function definition
bool GFEDigitalRead(int _pin)					{ return digitalRead(_pin);	}
int  GFEAnalogRead(int _pin)					{ return analogRead(_pin);	}
void GFEDigitalWrite(int _pin, bool _out)		{ digitalWrite(_pin, _out); }
void GFEAnalogWrite(int _pin, int _out)			{ analogWrite(_pin, _out);	}

void updateStatusLED(){
	// TODO : update internal LED status
}

void sendToActionBoard(){
	for(int i=0; i<NUM_OF_OUTPUT; i++){
		Serial.print(outputData[i], HEX);
		Serial.print(",");
	}
	Serial.println();
}

void sendToP5(){
	// TODO : send to Processing
	// start?

}