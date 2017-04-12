/*
 * Project localOSCTest
 * Description: test OSC without CLOUD
 * Author: Yi donghoon	
 * Date: 2017.04.11
 */

// SYSTEM_THREAD(ENABLED);


#include "simple-OSC.h"
SYSTEM_MODE(MANUAL);

UDP udp;
IPAddress outIP(192, 168, 100, 102);
unsigned int outPort = 8000;

unsigned int millisCounter;
unsigned int lastLEDToggleTimer;
bool bLEDMode = false;


void setup() {
	pinMode(D7, OUTPUT);
  // Put initialization like pinMode and begin functions here.
	Serial.begin(115200);

	// static IP setting
    // IPAddress myAddress(192, 168, 100, 101);           
    // IPAddress netmask(255, 255, 255, 0);
    // IPAddress gateway(192, 168, 100, 1);
    // IPAddress dns(192, 168, 100, 1);
    // WiFi.setStaticIP(myAddress, netmask, gateway, dns);

    // WiFi.useStaticIP();


    WiFi.connect();
    while(!WiFi.ready()){
    	Particle.process();		// 
    	delay(100);
	}
	Particle.process();			// ================================ must!!!!!!
	

	if(WiFi.ready()){	// flashing 10 times
		digitalWrite(D7, HIGH);
	}
    
	udp.begin(8001);

    Serial.print("local IP: \t");
    Serial.println(WiFi.localIP());

}


// loop() runs over and over again, as quickly as it can execute.
void loop() {
	// check ping to 
	Serial.println("OSC MESSAGE START!");
	OSCMessage message("/nemo");
	message.addFloat(1.23);
	message.send(udp, outIP, outPort);
	delay(500);
	Serial.println("OSC MESSAGE END!");

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