/*
    TODO : 

    [V] : fix pulse read failed -> heartRate makes 0
    [V] : tear detect
*/

#define LED             D7      //indicator, Grove - LED is connected with D4 of Arduino
#define ISRPIN          D2      // hearRate exInterrupt pin

// #define VOUTPIN         A0      // voltage out to tear sensor
#define TEAR_PIN           D4      // tear sensor input

#define FSR_INPUT_PIN   A1      // fsr sensor input
#define WHISPER_PIN     A2      // whisper sensor input

#include "simple-OSC.h"

SYSTEM_MODE(MANUAL);

UDP udp;

IPAddress outIP(192, 168, 100, 255);
unsigned int outPort = 8000;


volatile bool ledState = LOW;   

int iTearOn = 0;

unsigned int counter;
unsigned long temp[7];
unsigned long sub;
unsigned int fsrRead;
unsigned int whisperRead;
bool dataOK = true;

char sendBuffer[3];

unsigned int heartRate=0;
unsigned long lastPublishedTimer;
unsigned long lastHeartRateCheckedTimer;
const long maxHeartRatePulseDuty = 2000;

void setup() {
    // static IP setting
    IPAddress myAddress(192, 168, 100, 101);           
    IPAddress netmask(255, 255, 255, 0);
    IPAddress gateway(192, 168, 100, 1);
    IPAddress dns(192, 168, 100, 1);
    WiFi.setStaticIP(myAddress, netmask, gateway, dns);

    WiFi.useStaticIP();

    WiFi.connect();

    
    while(!WiFi.ready()){
    	Particle.process();
        delay(500);
    }
    Particle.process();

    udp.begin(8001);

    pinMode(LED, OUTPUT);
    pinMode(TEAR_PIN, INPUT);

    delay(5000);
    arrayInit();
    attachInterrupt(digitalPinToInterrupt(ISRPIN), interrupt, RISING);

    lastHeartRateCheckedTimer = lastPublishedTimer = millis();
}

void loop() {
    digitalWrite(LED, ledState);//Update the state of the indicator
    fsrRead = analogRead(FSR_INPUT_PIN);
    
    // check pulseOK .. if no external interrupt happen in 6 seconds... heartRate will be 0 which means something wrong.
    if(millis() - lastHeartRateCheckedTimer > 10000)     heartRate = 0;
    
    // osc publish : heartRate
    // send to feather
    if(millis() - lastPublishedTimer > 2000){
        Particle.publish("HeartRate", intToString(heartRate));

        // publish heartRate via OSC 
        OSCMessage outMessage("/demo/heartRate");
        outMessage.addInt(heartRate);
        outMessage.send(udp,outIP,outPort);

        //send to feather.. 
        // deprecated..
        // Wire.beginTransmission(8);
        // Wire.write(lowByte(heartRate));
        // Wire.write(highByte(heartRate));
        // Wire.endTransmission();

        lastPublishedTimer = millis();
    }
    
    // teardrop check
    if(digitalRead(TEAR_PIN)){
        iTearOn = 1;
    } else {
        iTearOn = 0;
    }
    
    whisperRead = analogRead(A2);
    
    // osc publish : teardrop
    OSCMessage tearMessage("/demo/tear");
    tearMessage.addInt(iTearOn);
    tearMessage.send(udp, outIP, outPort);

    OSCMessage fsrMessage("/demo/fsr");
    fsrMessage.addInt(fsrRead);
    fsrMessage.send(udp, outIP, outPort);   
    
    OSCMessage whisperMessage("/demo/whisper");
    whisperMessage.addInt(whisperRead);
    whisperMessage.send(udp, outIP, outPort);

    if(WiFi.ready()){
		digitalWrite(D7, HIGH);
	} else {
		udp.endPacket();
		udp.stop();
		digitalWrite(D7, LOW);
		
		WiFi.disconnect();

		// IPAddress myAddress(192, 168, 100, 101);           
	 //    IPAddress netmask(255, 255, 255, 0);
	 //    IPAddress gateway(192, 168, 100, 1);
	 //    IPAddress dns(192, 168, 100, 1);
	 //    WiFi.setStaticIP(myAddress, netmask, gateway, dns);

	 //    WiFi.useStaticIP();
		WiFi.off();
		delay(5000);
		WiFi.on();

		WiFi.connect();
		Serial.print("connecting");
		while(WiFi.connecting()){
			Serial.print("waiting connection... ");
			delay(1000);
		}
		Serial.println();

		while(!WiFi.ready()){
			Particle.process();	
			Serial.print(".");
    		delay(100);
    	}
    	Particle.process();


    	Serial.println();
    	Serial.println("WiFi fully ready.");

    	Particle.process();
    	Serial.print("PING returns : ");
    	Serial.println(WiFi.ping(WiFi.gatewayIP()));


    	Serial.println("========");
    	Serial.print("local IP: \t");
    	Serial.println(WiFi.localIP());

    	delay(5000);
       	udp.begin(8001);	//
	}
}

void getHeartRate(){
    if(dataOK){
        heartRate = 360000 / (temp[6] - temp[0]);
    } 
    lastHeartRateCheckedTimer = millis();
}

void interrupt(){
    ledState = !ledState;

    noInterrupts();
    // detatchInterrupts();

    temp[counter] = millis();

    if(counter == 0)    sub = temp[counter] - temp[6];
    else                sub = temp[counter] - temp[counter-1];

    if(sub > maxHeartRatePulseDuty){
        dataOK = false;
        counter = 0;
        heartRate = 0;                      // interrupt should be occur for this check routine!!! 
        arrayInit();
    } 
    
    if(counter == 6 && dataOK){
        counter = 0; 
        getHeartRate();
    }  else if (counter!= 6 && dataOK){
        counter++;
        // ledState != ledState;
    } else {
        counter=0;
        dataOK = true;
    }
    interrupts();
    // attachInterrupt()
}

void arrayInit(){
    for(int i=0; i<6; i++){
        temp[i] = 0;
        temp[6] = millis();
    }
}

String intToString(int num){
    char message[64] = "";
    sprintf(message, "%d", num);
    return message;
}
