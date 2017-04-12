// This #include statement was automatically added by the Particle IDE.
#include <simple-OSC.h>

#define INPUT_PIN   A3

UDP udp;
IPAddress outIP(192,168,0,40);
unsigned int outPort = 8000;

unsigned int analogValue=0;

unsigned long lastPublishedTimer=0;

void setup() {
	IPAddress myAddress(192, 168, 0, 80);
	IPAddress netmask(255, 255, 255, 0);
	IPAddress gateway(192, 168, 0, 1);
	IPAddress dns(8, 8, 8, 8);

	WiFi.setStaticIP(myAddress, netmask, gateway, dns);

	WiFi.useStaticIP();

	udp.begin(115200);
	while(!WiFi.ready()){
		delay(5000);
	}

	IPAddress ip = WiFi.localIP();

	char message[64] = "";
	sprintf(message, "My Local IP= %d:%d:%d:%d", ip[0], ip[1], ip[2], ip[3]);
	Particle.publish("IP", message);
}

void loop() {
	analogValue = analogRead(INPUT_PIN);

	// publish analogValue in every 2 seconds
	if(millis() - lastPublishedTimer > 2000){
		Particle.publish("analogRead", int2String(analogValue));
		lastPublishedTimer = millis();
	}

	OSCMessage outMessage("/core/analogValue");
	outMessage.addInt(analogValue);
	outMessage.send(udp, outIP, outPort);

}

String int2String(int num){
	char message[64] = "";
	sprintf(message, "%d", num);
	return message;
}

