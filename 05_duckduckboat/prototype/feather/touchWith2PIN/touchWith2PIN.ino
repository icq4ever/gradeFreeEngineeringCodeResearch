#define UP 		9
#define LED 	13
#define READ 	10

void setup(){
	pinMode(UP, OUTPUT);
	pinMode(LED, OUTPUT);
	pinMode(READ, INPUT);

	digitalWrite(UP, HIGH);
}

void loop(){
	if(digitalRead(READ)==HIGH)	{
		digitalWrite(LED, HIGH);
	} else {
		digitalWrite(LED, LOW);
	}

	// digitalWrite(LED, HIGH);
	// delay(1000);
	// digitalWrite(LED, LOW);
	// delay(1000);
}