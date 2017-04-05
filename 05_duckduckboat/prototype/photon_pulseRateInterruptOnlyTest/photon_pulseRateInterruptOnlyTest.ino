#define LED     D7					
#define ISRPIN  D2
volatile bool ledState = LOW;		

void setup() {
	pinMode(LED, OUTPUT);
	attachInterrupt(ISRPIN, interrupt, CHANGE);
}

void loop() {
	digitalWrite(LED, ledState);	
}

void interrupt(){
    ledState = !ledState;
}

