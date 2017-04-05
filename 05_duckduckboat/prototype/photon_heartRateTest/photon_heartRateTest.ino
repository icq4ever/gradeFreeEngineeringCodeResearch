#define LED     D7                  //indicator, Grove - LED is connected with D4 of Arduino
#define ISRPIN  D2

#include "simple-OSC.h"

UDP udp;

IPAddress outIP(192, 168, 100, 103);
unsigned int outPort = 8000;


bool bPulseWorking = false;
volatile bool ledState = LOW;       

unsigned int counter;
unsigned long temp[7];
unsigned long sub;
bool dataOK = true;

unsigned int heartRate=0;
unsigned long lastPublishedTimer = 0;
const long maxHeartRatePulseDuty = 2000;

void setup() {
    // static IP setting
    IPAddress myAddress(192, 168, 100, 101);            // 102 : donghoon MBP
    IPAddress netmask(255, 255, 255, 0);
    IPAddress gateway(192, 168, 100, 1);
    IPAddress dns(8,8,8,8);
    WiFi.setStaticIP(myAddress, netmask, gateway, dns);
    
    WiFi.useStaticIP();
    
    udp.begin(115200);
    while(!WiFi.ready()){
        delay(500);
    }
    
    IPAddress ip = WiFi.localIP();
    
    char message[64] = "";
    sprintf(message, "My Local IP= %d:%d:%d:%d", ip[0], ip[1], ip[2], ip[3]);
    Particle.publish("myLocalIP", message);
    
    pinMode(LED, OUTPUT);
    delay(5000);
    arrayInit();
    Particle.publish("LOG", "heartRate test begin...");
    attachInterrupt(ISRPIN, interrupt, RISING);
    
    lastPublishedTimer = millis();
}

void loop() {
    digitalWrite(LED, ledState);//Update the state of the indicator
    
    if(millis() - lastPublishedTimer > 2000){
        Particle.publish("HeartRate", intToString(heartRate));
        
        // publish heartRate via OSC 
        OSCMessage outMessage("/heartRate/demo");
        outMessage.addInt(heartRate);
        outMessage.send(udp,outIP,outPort);
        lastPublishedTimer = millis();
    }
    
    
}

void getHeartRate(){
    if(dataOK){
        heartRate = 360000 / (temp[6] - temp[0]);
    } 
}

void interrupt(){
    ledState = !ledState;
    
    noInterrupts();
    
    temp[counter] = millis();
    
    if(counter == 0)    sub = temp[counter] - temp[6];
    else                sub = temp[counter] - temp[counter-1];
    
    if(sub > maxHeartRatePulseDuty){
        dataOK = false;
        counter = 0;
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
}

void arrayInit(){
    heartRate = 0;
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
