/*
    dummyOSCBroadcaster

    braodcasting message with 10ms delay, repeatly
*/


#include "simple-OSC.h"

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#define LED             D7      //indicator, Grove - LED is connected with D4 of Arduino
UDP udp;

IPAddress outIP(192, 168, 100, 255);        // braodcast ip addess
unsigned int outPort = 8000;                // port

unsigned int count;

bool ledState = LOW;   
unsigned long lastLEDStateChangedTimer;

void setup() {
	Serial.begin(115200);
    // static IP setting
	IPAddress myAddress(192, 168, 100, 104);           
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

	Serial.println("OSC address patterns... : ");
}

void loop() {
	count++;

	sendOSCMessage("/sy", 1, 2, 3, 4, 5, count);
	Particle.process();

	// LED status 
	if(WiFi.ready())	blinkingLED(50);
	else {
		blinkingLED(200);
		delay(3000);
		System.reset();
	}	

}

void blinkingLED(int interval){
	if(millis() - lastLEDStateChangedTimer > interval){
		ledState = !ledState;
		lastLEDStateChangedTimer = millis();
	}
	digitalWrite(LED, ledState);
}

void sendOSCMessage(String addrPattern, int value){
	OSCMessage message(addrPattern);
	message.addString(int2String(value));
	message.send(udp, outIP, outPort);
	delay(10);	
}

void sendOSCMessage(String addrPattern, int value1, int value2){
	OSCMessage message(addrPattern);
	message.addString(int2String(value1));
	message.addString(int2String(value2));
	message.send(udp, outIP, outPort);
	delay(10);	
}

void sendOSCMessage(String addrPattern, int value1, int value2, int value3){
	OSCMessage message(addrPattern);
	message.addString(int2String(value1));
	message.addString(int2String(value2));
	message.addString(int2String(value3));
	message.send(udp, outIP, outPort);
	delay(10);	
}

void sendOSCMessage(String addrPattern, int value1, int value2, int value3, int value4){
	OSCMessage message(addrPattern);
	message.addString(int2String(value1));
	message.addString(int2String(value2));
	message.addString(int2String(value3));
	message.addString(int2String(value4));
	message.send(udp, outIP, outPort);
	delay(10);	
}


void sendOSCMessage(String addrPattern, int value1, int value2, int value3, int value4, int value5){
	OSCMessage message(addrPattern);
	message.addString(int2String(value1));
	message.addString(int2String(value2));
	message.addString(int2String(value3));
	message.addString(int2String(value4));
	message.addString(int2String(value5));
	message.send(udp, outIP, outPort);
	delay(10);	
}

void sendOSCMessage(String addrPattern, int value1, int value2, int value3, int value4, int value5, int value6){
	OSCMessage message(addrPattern);
	message.addString(int2String(value1));
	message.addString(int2String(value2));
	message.addString(int2String(value3));
	message.addString(int2String(value4));
	message.addString(int2String(value5));
	message.addString(int2String(value6));
	message.send(udp, outIP, outPort);
	delay(10);	
}

String int2String(int num){
	char message[64] = "";
	sprintf(message, "%d", num);
	return message;
}

String double2String(unsigned long num){
	char message[64] = "";
	sprintf(message, "%lu", num);
	return message;
}

String float2String(float num){
	char message[64] = "";
	sprintf(message, "%f", num);
	return message;
}