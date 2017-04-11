/*
 * Project localOSCTest
 * Description: test OSC without CLOUD
 * Author: Yi donghoon	
 * Date: 2017.04.11
 */

// SYSTEM_THREAD(ENABLED);


#include "simple-OSC.h"
SYSTEM_MODE(SEMI_AUTOMATIC);

UDP udp;
IPAddress outIP(192, 168, 100, 102);
unsigned int outPort = 8008;

unsigned int millisCounter;
unsigned int lastLEDToggleTimer;
bool bLEDMode = false;


// STARTUP(semiAutomaticSetting());
// setup() runs once, when the device is first turned on.
// 
// void semiAutomaticSetting(){

// }


void setup() {
	pinMode(D7, OUTPUT);
  // Put initialization like pinMode and begin functions here.
	Serial.begin(115200);

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
    	delay(100);
	}

	WiFi.ping(WiFi.gatewayIP());

	if(WiFi.ready()){	// flashing 10 times
		for(int i=0; i<10; i++){
    		digitalWrite(D7, HIGH);
    		delay(100);
    		digitalWrite(D7, LOW);	
    		delay(100);
    	}
	}
    
	udp.begin(8001);

    millisCounter = millis();
    lastLEDToggleTimer = millis();
    Serial.print("local IP: \t");
    Serial.println(WiFi.localIP());

}


// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
	// millisCounter = millis();

	if (millis() - lastLEDToggleTimer > 200){
		bLEDMode = !bLEDMode;
		lastLEDToggleTimer = millis();
		millisCounter ++;
		Serial.println();
		Serial.print("counter : \t");
		Serial.println(millisCounter);

		WiFi.ping(WiFi.gatewayIP());

		OSCMessage message("/demo/millis");
		message.addInt(300);
		message.send(udp, outIP, outPort);
		Serial.print(".");
	}

	digitalWrite(D7, bLEDMode);
}