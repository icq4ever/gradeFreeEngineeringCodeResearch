#define ANALOG_PIN 0
#define BUTTON_PIN 2


// digital 값은 1/0으로 
// analog 값은 고대로 날린다.
// 이후 프로세싱에서 String으로 받아서 숫자로 변환해서 쓰게 하자.

int aValue = 0;
int bValue = 0;
int digitalButton = 0;

// the setup function runs once when you press reset or power the board
void setup() {
	// initialize digital pin LED_BUILTIN as an output.
	Serial.begin(115200);
	pinMode(BUTTON_PIN, INPUT);
}

// the loop function runs over and over again forever
void loop() {
	aValue = analogRead(ANALOG_PIN);	// read A0
	bValue = random(500, 524);			// random value ~12

	if(digitalRead(BUTTON_PIN))     digitalButton = 1;
	else                            digitalButton = 0;

	// read from A9
	Serial.print(digitalButton, HEX);
	Serial.print(",");
	Serial.print(aValue, HEX);		// analog Value #1
	Serial.print(",");				// split character
	Serial.print(bValue, HEX);		// analog value #2
	Serial.println();				// end of send Value!!

	// send mapped between 0-1023 from generate sine value
	// Serial.println(sin(millis()));
}