/*
    
 souceCode by GradeFreeEngineering team.
 
 
 [image License]
 duck pixel icon is from "http://piq.codeus.net/picture/370697/rubber_ducky"
 by ZacharyB
 free for cormmercial use. 

 Platformer Art Deluxe (Pixel Redux) by Kenney Vleugels (www.kenney.nl)
 License (CC0)

 TODO :
 [ ] : add to feather (!while) in serial.

 */


import controlP5.*;
import oscP5.*;
import netP5.*;
import processing.serial.*;


ControlUIFrame cf;

OscP5 oscP5;
NetAddress remoteClients;

TwoDimensionUI controlUI;
Serial feather;

PImage duckImage, waterImage;

int throttleValue;  // -100 ~ 100
int handlingValue;  // -100 ~ 100
int keyThrottleValue = 0;
boolean bManualControlEnabled = false;
boolean bKillEnabled = true;



PFont font, smallFont;

int lastGoCmdTimer, lastBackCmdTimer, lastLeftCmdTimer, lastRightCmdTimer, lastLORASentTimer;

int accelCountBuffer[] = new int[10];
int handlingCountBuffer[] = new int[10];
int accelCount, handlingCount;
int tmpAccelCount, tmpHandlingCount;
int tmpGoBackCount;
int lastMessageCountCheckedTimer;
int bufferIndex = 0;

void settings(){
    size(300, 300);   
}

void setup() {
    //size(300,300);
    frame.setLocation(0, 360);
    frameRate(60);
    
    duckImage = loadImage("duck.png");
    waterImage = loadImage("water.png");
    
    cf = new ControlUIFrame(this, 300, 300, "Control");
    font = loadFont("ShareTechMono-Regular-24.vlw");
    smallFont = loadFont("ShareTechMono-Regular-14.vlw");
    
    
    oscP5 = new OscP5(this, 8000);
    remoteClients = new NetAddress("192.168.100.255", 8001); //????????????????????????????????????????????????????????????????????????????????????????????

    for (int i=0; i<Serial.list().length; i++) {
        println("[" + i + "] : " + Serial.list()[i]);
    }
    
    lastMessageCountCheckedTimer = lastLORASentTimer = millis();
    for(int i=0; i<accelCountBuffer.length; i++)    {
        accelCountBuffer[i] = 0;
        handlingCountBuffer[i] = 0;
        
    }
    controlUI = new TwoDimensionUI("controlUI", 300);    
    //feather = new Serial(this, "COM11", 9600);
    feather = new Serial(this, Serial.list()[1], 9600);
    
    tmpAccelCount = 0;
    tmpHandlingCount = 0;
    tmpGoBackCount = 0;
    
}

void draw() {
    background(0);
    control();
    controlUI.draw(width/2, height/2);

    accelCount = handlingCount = 0;

    // fill buffer
    if(millis() - lastMessageCountCheckedTimer > 50){
    //    if(millis() - lastMessageCountCheckedTimer > 100){

        accelCountBuffer[bufferIndex] = tmpAccelCount;
        handlingCountBuffer[bufferIndex] = tmpHandlingCount;

        tmpAccelCount = tmpHandlingCount = 0;
        bufferIndex++;

        if(bufferIndex >= accelCountBuffer.length)  bufferIndex = 0;
        lastMessageCountCheckedTimer = millis();
    }

    // calc
    for(int i=0; i<accelCountBuffer.length; i++){
        accelCount += accelCountBuffer[i];
        handlingCount  += handlingCountBuffer[i];
    }

    pushStyle();
    textFont(smallFont);
    text("accel     : " +  accelCount, 10, 20);
    text("handling  : " +  handlingCount, 10, 40);
    popStyle();


    // send to LoRa
    if (millis() - lastLORASentTimer > 100) send2LoRA();
    
    if(bManualControlEnabled){
        pushStyle();
        textFont(font);
        textAlign(LEFT, BOTTOM);
        fill(#FFFF00);
        text("MANUAL", 20, height-20);
        popStyle();
    }
    
    if(bKillEnabled){
        pushStyle();
        textFont(font);
        textAlign(RIGHT, BOTTOM);
        fill(#33FF00);
        text("OFFLINE", width-20, height-20);
        popStyle();
    }
}

void keyPressed(){
    if(key == '.' || key == '>'){
        keyThrottleValue++;
        if(keyThrottleValue > 100)      keyThrottleValue = 100;
    } else if (key == ',' || key == '<'){
        keyThrottleValue--;
        if(keyThrottleValue < -100)     keyThrottleValue = -100;
    }
    
}

void keyReleased() {
    if(key == 'z' || key == 'Z'){
        bKillEnabled = !bKillEnabled;
        
        if (!bKillEnabled) {
            cf.toggleOffline.setValue(false);
            handlingValue = 0;
            throttleValue = 0 ;
        } else {
            cf.toggleOffline.setValue(true);
        }
        handlingValue = 0;
        keyThrottleValue = 0;
        
    }

    if(key == ' '){
        bManualControlEnabled = !bManualControlEnabled;
        
        if (!bManualControlEnabled) { 
            cf.toggleControlOverride.setValue(false);
        } else {
            cf.toggleControlOverride.setValue(true);
        }
        handlingValue = 0;
        keyThrottleValue = 0;
    }    
}

void send2LoRA(){
     byte message[] = new byte[4];
        
        message[0] = '/';
        //message[1] = (byte)(-100);
        //message[2] = (byte)(260);
        if(bKillEnabled){
            message[1] = (byte)(100);
            message[2] = (byte)(100);
        } else {
            message[1] = (byte)(throttleValue + 100);
            message[2] = (byte)(handlingValue + 100);
        }
        
        feather.write(message);
        
        //println("THROTTLE Angle : " + int(map(throttleValue, -100, 100, SERVOMIN, SERVOMAX)) + "\t " + 
        //        "HANDING Angle :"   + int(map(handlingValue, -100, 100, SERVOMIN, SERVOMAX)));
        lastLORASentTimer = millis();
        
        //println(keyThrottleValue);
}

void control() {
    if(bManualControlEnabled){
        throttleValue = keyThrottleValue;
    } else {
        throttleValue = (int)constrain(map(accelCount, -50, 50, -100, 100), -100, 100);
        handlingValue = (int)constrain(map(handlingCount, -10, 10, -100, 100), -100, 100);
        
        if(handlingValue < 0)            handlingValue++;
        else if(handlingValue > 0)       handlingValue--;
    }

    if(bKillEnabled){
        throttleValue = 0;
        handlingValue = 0;
    }
    
    PVector temp = new PVector(handlingValue, throttleValue);
    controlUI.update(temp);     // UI update
}

void serialEvent(Serial port) {
    if (port == feather) {
        // TODO : 
        // do something
        //String inString = port.readStringUntil('\n'); 
    }
}


// check oscMessage 
void oscEvent(OscMessage incommingMessage) {
    if(incommingMessage.checkAddrPattern("/gofwd"))     tmpAccelCount++;
    if(incommingMessage.checkAddrPattern("/left"))      tmpHandlingCount--;
    if(incommingMessage.checkAddrPattern("/right"))     tmpHandlingCount++;
    if(incommingMessage.checkAddrPattern("/goback"))    tmpAccelCount=tmpAccelCount - 4;
}

// only works when manualControl enable
void mouseMoved(){
    if(bManualControlEnabled) handlingValue = (int)constrain(map(mouseX, 0, width, -100, 100), -100, 100); 
}