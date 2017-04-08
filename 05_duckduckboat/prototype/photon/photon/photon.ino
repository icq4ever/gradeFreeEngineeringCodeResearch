/*
    TODO : 

    [V] : fix pulse read failed -> heartRate makes 0
    [V] : tear detect
*/

#define LED             D7      //indicator, Grove - LED is connected with D4 of Arduino
#define ISRPIN          D2      // hearRate exInterrupt pin

#define VOUTPIN         A0      // voltage out to tear sensor
#define INPIN           D4      // tear sensor input

#define FSR_INPUT_PIN   A1      // fsr sensor input

#include "simple-OSC.h"

UDP udp;

IPAddress outIP(192, 168, 100, 102);
unsigned int outPort = 8000;


volatile bool ledState = LOW;   

int iTearOn = 0;

unsigned int counter;
unsigned long temp[7];
unsigned long sub;
unsigned int fsrRead;
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

    // I2C setting
    Wire.begin();   // join i2c bus as master

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
        Wire.beginTransmission(8);
        Wire.write(lowByte(heartRate));
        Wire.write(highByte(heartRate));
        Wire.endTransmission();

        lastPublishedTimer = millis();
    }
    
    // teardrop check
    if(digitalRead(INPIN)){
        iTearOn = 1;
    } else {
        iTearOn = 0;
    }
    
    // osc publish : teardrop
    OSCMessage tearMessage("/demo/tear");
    tearMessage.addInt(iTearOn);
    tearMessage.send(udp, outIP, outPort);

    OSCMessage fsrMessage("/demo/fsr");
    fsrMessage.addInt(fsrRead);
    fsrMessage.send(udp, outIP, outPort);    
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
