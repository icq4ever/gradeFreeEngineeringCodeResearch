/*
    TODO : 

    [V] : fix pulse read failed -> heartRate makes 0
    [V] : tear detect
*/

#define LED     D7                  //indicator, Grove - LED is connected with D4 of Arduino
#define ISRPIN  D2

#define VOUTPIN A0
#define INPIN   D4

#include "simple-OSC.h"

UDP udp;

IPAddress outIP(192, 168, 100, 103);
unsigned int outPort = 8000;


volatile bool ledState = LOW;   

int iTearOn = 0;

unsigned int counter;
unsigned long temp[7];
unsigned long sub;
bool dataOK = true;

unsigned int heartRate=0;
unsigned long lastPublishedTimer;
unsigned long lastHeartRateCheckedTimer;
const long maxHeartRatePulseDuty = 2000;

void setup() {
    // static IP setting
    IPAddress myAddress(192, 168, 100, 101);           
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
    
    pinMode(VOUTPIN, OUTPUT);
    pinMode(INPIN, INPUT);
    digitalWrite(VOUTPIN, HIGH);
    
    delay(5000);
    arrayInit();
    Particle.publish("LOG", "heartRate test begin...");
    attachInterrupt(ISRPIN, interrupt, RISING);
    
    lastHeartRateCheckedTimer = lastPublishedTimer = millis();
}

void loop() {
    digitalWrite(LED, ledState);//Update the state of the indicator
    
    // check pulseOK 
    if(millis() - lastHeartRateCheckedTimer > 3000)     heartRate = 0;
    
    
    // osc publish : heartRate
    if(millis() - lastPublishedTimer > 2000){
        Particle.publish("HeartRate", intToString(heartRate));
        
        // publish heartRate via OSC 
        OSCMessage outMessage("/demo/heartRate");
        outMessage.addInt(heartRate);
        outMessage.send(udp,outIP,outPort);
        lastPublishedTimer = millis();
    }
    
    // teardrop check
    if(digitalRead(INPIN)){
        iTearOn = 1;
    } else {
        iTearOn = 0;
    }
    
    // osc publish : teardrop
    OSCMessage outMessage("/demo/tear");
    outMessage.addInt(iTearOn);
    outMessage.send(udp, outIP, outPort);
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
