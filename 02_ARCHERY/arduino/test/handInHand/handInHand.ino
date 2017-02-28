const byte ledPin = 13;
const byte interruptPin1 = 2;
const byte interruptPin2 = 3;
volatile byte state = LOW;
volatile unsigned int countNum; 

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin1, INPUT_PULLUP);
  pinMode(interruptPin2, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin1), blink, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), count, FALLING);
  Serial.begin(115200);

  countNum = 0;
}

void loop() {
  // digitalWrite(ledPin, state);
  // Serial.println(countNum);

  delay(30);
  
  if(countNum > 0)	countNum--;

  if(countNum > 30)   digitalWrite(ledPin, HIGH);
  else                digitalWrite(ledPin, LOW);
}

void blink() {
  state = !state;
}

void count(){
	if(countNum < 40)  countNum++;
  else               countNum = 40;
}