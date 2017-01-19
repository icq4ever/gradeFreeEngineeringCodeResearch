#define PIN_START_BUTTON 	6
#define PIN_BLOWER_1		A2
#define PIN_BLOWER_2		A3
#define PIN_BLOWER_3		A4
#define PIN_HEATING_MBALL	9
#define PIN_RELEASE_MBALL	10
#define PIN_EGG_BREAKER		11
#define PIN_NOODLE_UP		12
#define PIN_NOODLE_DOWN		13

#define NUM_OF_OUTPUT		9

int outputPin[NUM_OF_OUTPUT];		// store pins
int outputData[NUM_OF_OUTPUT];		// store input of pin

void setup() {
	// start Serial vis USB
	Serial.begin(115200);

	outputPin[0]=PIN_START_BUTTON;
	outputPin[1]=PIN_BLOWER_1;	
	outputPin[2]=PIN_BLOWER_2;	
	outputPin[3]=PIN_BLOWER_3;
	outputPin[4]=PIN_HEATING_MBALL;
	outputPin[5]=PIN_RELEASE_MBALL;
	outputPin[6]=PIN_EGG_BREAKER;
	outputPin[7]=PIN_NOODLE_UP;
	outputPin[8]=PIN_NOODLE_DOWN;

	for(int i=0; i<9; i++){
		outputData[i] = 0;
		pinMode(outputPin[i], INPUT);
	}
}

void loop() {

	for(int i=0; i<NUM_OF_OUTPUT; i++){
		if(GFEDigitalRead(outputPin[i]))	outputData[i] = 1;
		else								outputData[i] = 0;
	}

	printDataToSerial();
}

// sudo function definition
bool GFEDigitalRead(int _pin)					{ return digitalRead(_pin);	}
int  GFEAnalogRead(int _pin)					{ return analogRead(_pin);	}
void GFEDigitalWrite(int _pin, bool _out)		{ digitalWrite(_pin, _out); }
void GFEAnalogWrite(int _pin, int _out)			{ analogWrite(_pin, _out);	}


void printDataToSerial(){
	for(int i=0; i<NUM_OF_OUTPUT; i++){
		Serial.print(outputData[i], HEX);
		Serial.print(",");
	}
	Serial.println();
}
