/* 
	particle photon example!!! 
*/

#include "simple-OSC.h"

UDP udp;

IPAddress outIp(192, 168, 100, 102);//your computer IP
unsigned int outPort = 8000; //computer incoming port

unsigned char counter;
unsigned long temp[7];
unsigned long sub;
bool dataOK = true;
unsigned int heartRate;
volatile bool ledState = LOW;

const int maxHeartRatePulseDuty = 2000;

void setup() {
    
    // static IP setting
    IPAddress myAddress(192, 168, 100, 101);
    IPAddress netmask(255, 255, 255, 0);
    IPAddress gateway(192, 168, 100, 1);
    IPAddress dns(192, 168, 100, 1);
    WiFi.setStaticIP(myAddress, netmask, gateway, dns);
    
    WiFi.useStaticIP();
 
    Serial.begin(115200);
    udp.begin(0);//necessary even for sending only.
    while (!WiFi.ready()) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    
    IPAddress ip = WiFi.localIP();
  
    char message[64] = "";
    sprintf(message, "My Local IP= %d:%d:%d:%d", ip[0], ip[1], ip[2], ip[3]);
    Particle.publish("myLocalIP", message);
    
    // setup heartRate
    delay(5000);
    pinMode(D7, OUTPUT);
    pinMode(D3, INPUT);
    Particle.publish("LOG", "heartRate test begin");
    attachInterrupt(D3, heart, RISING);
}

void loop() {
    digitalWrite(D7, ledState);
}

void getHeartRate(){
    if(dataOK){
        heartRate = 360000/(temp[6] - temp[0]);
        Particle.publish("HeartRate", heartRate);
        
        //SEND
        OSCMessage outMessage("/pulseRate");
        outMessage.addFloat(heartRate);
        outMessage.send(udp,outIp,outPort);
        // delay(3000);
    }
}

void heart(){
    noInterrupts();
    Particle.publish("LOG", "HEART!");
    temp[counter] = millis();
    
    switch(counter){
        case 0:
            sub = temp[counter] - temp[6];
            // Particle.publish("sub", sub);
            break;
        default:
            sub = temp[counter] - temp[counter-1];
            // Particle.publish("sub", sub);
            break;
    }
    
    if(sub>maxHeartRatePulseDuty) {
        dataOK = false;
        counter = 0;
        // Particle.publish("LOG", "HeartRate measure failed... restart!");
        arrayInit();
    }
    
    if(counter == 6 && dataOK){
        counter =0;
        // getHeartRate();
    }
    
    else if(counter != 6 && dataOK){
        counter++;
        ledState = !ledState;
    }
    
    else {
        counter = 0;
        dataOK = false;
    }
    interrupts();
}

void arrayInit(){
    for(unsigned int i=0; i<6; i++){
        temp[i] = 0;
        temp[6] = millis();
    }
}
