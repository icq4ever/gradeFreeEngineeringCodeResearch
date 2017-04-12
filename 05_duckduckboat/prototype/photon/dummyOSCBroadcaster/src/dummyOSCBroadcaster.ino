/*
    dummyOSCBroadcaster
*/


#include "simple-OSC.h"

SYSTEM_MODE(MANUAL);
#define LED             D7      //indicator, Grove - LED is connected with D4 of Arduino

UDP udp;

IPAddress outIP(192, 168, 100, 255);        // braodcast ip addess
unsigned int outPort = 8000;                // port

volatile bool ledState = LOW;   

#define ADDRSIZE	50
String addressbook[ADDRSIZE];

unsigned long count = 0;

void setup() {
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
		delay(500);
	}
	Particle.process();

	udp.begin(8001);

	pinMode(LED, OUTPUT);

	// addressbook = new String[10];

	Serial.println("OSC address patterns... : ");

	// for(int i=0; i< sizeof(addressbook) / sizeof(addressbook[0]); i++){
		// String tmpName = "/GFE_2";
		// if(i<10)	addressbook[i] = tmpName + "0" + String(i);
		// else		addressbook[i] = tmpName + String(i);
		// Serial.println(addressbook[i]);
	// }
}

void loop() {
    digitalWrite(LED, HIGH);//Update the state of the indicator
    
    // for(int i=0; i<sizeof(addressbook) / sizeof(addressbook[0]); i++){
    // 	OSCMessage message(addressbook[i]);
    // 	message.addInt(millis() + i);
    // 	message.send(udp, outIP, outPort);
    // 	delay(10);
    // }

    
    	OSCMessage message("/GFE_1");
    	// for(int i=0; i<sizeof(addressbook) / sizeof(addressbook[0]); i++){
    		message.addString(double2String(count));
    	// }
    	message.send(udp, outIP, outPort);
    	count++;
    	delay(10);
    

    digitalWrite(LED, LOW);

   //  if(WiFi.ready()){
   //  	digitalWrite(D7, HIGH);
   //  } else {
   //  	udp.endPacket();
   //  	udp.stop();
   //  	digitalWrite(D7, LOW);

   //  	WiFi.disconnect();

   //  	WiFi.off();
   //  	delay(5000);
   //  	WiFi.on();

   //  	WiFi.connect();
   //  	Serial.print("connecting");
   //  	while(WiFi.connecting()){
   //  		Serial.print("waiting connection... ");
   //  		delay(1000);
   //  	}
   //  	Serial.println();

   //  	while(!WiFi.ready()){
   //  		Particle.process();	
   //  		Serial.print(".");
   //  		delay(100);
   //  	}
   //  	Particle.process();


   //  	Serial.println();
   //  	Serial.println("WiFi fully ready.");

   //  	Particle.process();
   //  	Serial.print("PING returns : ");
   //  	Serial.println(WiFi.ping(WiFi.gatewayIP()));


   //  	Serial.println("========");
   //  	Serial.print("local IP: \t");
   //  	Serial.println(WiFi.localIP());

   //  	delay(5000);
   //     	udp.begin(8001);	//
   //     }
   // }
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