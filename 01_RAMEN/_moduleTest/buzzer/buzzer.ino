#define PIN_SIREN		A4
#define PIN_BUZZER_PWM	11

void setup(){
	
}

void loop(){
	buzzer();
}

void buzzer(){
	if((micros()%BUZZER_MICROHERZ*BUZZER_DUTYCYCLE) < BUZZER_MICROHERZ*BUZZER_DUTYCYCLE)	bBuzzerHW = true;
	else																					bBuzzerHW = false;
	analogWrite(A4, bBuzzerHW);
}