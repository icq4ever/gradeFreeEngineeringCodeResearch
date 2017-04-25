/*
    
 souceCode by GradeFreeEngineering team.
 
 
 [image License]
 duck pixel icon is from "http://piq.codeus.net/picture/370697/rubber_ducky"
 by ZacharyB
 free for cormmercial use. 

 Platformer Art Deluxe (Pixel Redux) by Kenney Vleugels (www.kenney.nl)
 License (CC0)

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

boolean bMouseControlEnabled = false;
boolean bKillEnabled = true;

PFont font;

int lastGoCmdTimer, lastBackCmdTimer, lastLeftCmdTimer, lastRightCmdTimer, lastLORASentTimer;


int accelCountBuffer[] = new int[10];
int bufferIndex = 0;
int accelCount;
int lastMessageCountCheckedTimer;
int tmpCount;

void settings(){
    size(300, 300);   
}

void setup() {
    //size(300,300);
    frame.setLocation(displayWidth, 360);
    frameRate(60);
    
    duckImage = loadImage("duck.png");
    waterImage = loadImage("water.png");
    
    cf = new ControlUIFrame(this, 300, 300, "Control");
    font = loadFont("ShareTechMono-Regular-24.vlw");
    oscP5 = new OscP5(this, 9000);
    remoteClients = new NetAddress("192.168.0.255", 9001);

    for (int i=0; i<Serial.list().length; i++) {
        println("[" + i + "] : " + Serial.list()[i]);
    }
    
    lastMessageCountCheckedTimer = lastGoCmdTimer = lastBackCmdTimer = lastLeftCmdTimer = lastRightCmdTimer = lastLORASentTimer = millis();
    for(int i=0; i<accelCountBuffer.length; i++)    accelCountBuffer[i] = 0;
    controlUI = new TwoDimensionUI("controlUI", 300);    
    feather = new Serial(this, Serial.list()[1], 9600);
}

void draw() {
    background(0);
    control();
    controlUI.draw(width/2, height/2);

    accelCount = 0;

    // fill buffer
    if(millis() - lastMessageCountCheckedTimer > 100){
        accelCountBuffer[bufferIndex] = tmpCount;
        tmpCount = 0;
        bufferIndex++;

        if(bufferIndex >= accelCountBuffer.length)  bufferIndex = 0;
        lastMessageCountCheckedTimer = millis();
    }

    // calc
    for(int i=0; i<accelCountBuffer.length; i++)    accelCount += accelCountBuffer[i];
    text(accelCount, 20, 20);


    // send to LoRa
    if (millis() - lastLORASentTimer > 100) send2LoRA();
    
    if(bMouseControlEnabled){
        pushStyle();
        textFont(font);
        textAlign(LEFT, BOTTOM);
        text("mouse\nenabled", 20, height-20);
        popStyle();
    }
    
    if(bKillEnabled){
        pushStyle();
        textFont(font);
        textAlign(RIGHT, BOTTOM);
        text("OFFLINE", width-20, height-20);
        popStyle();
    }
}

void keyReleased() {
    if(key == 'z' || key == 'Z'){
        bKillEnabled = !bKillEnabled;
        if (!bKillEnabled) {
            cf.toggleOffline.setValue(false);
        } else {
            cf.toggleOffline.setValue(true);
        }

        if (!bMouseControlEnabled) { 
            handlingValue = 0;
            cf.toggleMouseOverride.setValue(false);
        } else {
            cf.toggleMouseOverride.setValue(true);
        }
    }

    if(key == ' '){
        bMouseControlEnabled = !bMouseControlEnabled;
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
}

void keyPressed() {
    if (key == CODED) {
        if(keyCode == UP){
            throttleValue++;
            if(throttleValue > 100)		throttleValue = 100;
        }
        
        if(keyCode == DOWN){
            throttleValue--;
            if(throttleValue < -100)	throttleValue = -100;
        }
    }
}

void control() {
    // if (lastGoCmdTimer > lastBackCmdTimer) {
    //     // go Command
    //     if (millis() - lastGoCmdTimer < 150) {
    //         throttleValue++;
    //         if (throttleValue > 100)                  throttleValue = 100;
    //     } else {
    //         throttleValue = 0;
    //         if (throttleValue<0)                      throttleValue = 0;
    //     }
    // } else if (lastBackCmdTimer > lastGoCmdTimer) {
    //     // back Command
    //     if (millis() - lastBackCmdTimer < 100) {
    //         if (millis() % 2 == 0)                    throttleValue--;
    //         if (throttleValue<-100)                   throttleValue = -100;
    //     } else {
    //         //if(millis() % 1000 < 3) 
    //         throttleValue++;
    //         if (throttleValue > 0)                    throttleValue = 0;
    //     }
    // }


     //handling
    if(!bMouseControlEnabled){
        if (lastLeftCmdTimer > lastRightCmdTimer) {
            // left Command
            if (millis() - lastLeftCmdTimer < 120) {
                if (handlingValue > 0)                    handlingValue = 0;
                handlingValue = handlingValue - 3;
                if (handlingValue < -100)                 handlingValue = -100;
            } else {
                handlingValue = 0;
                if (handlingValue > 0)                    handlingValue = 0;
            }
        } else if (lastRightCmdTimer > lastLeftCmdTimer) {
            // right Command
            if (millis() - lastRightCmdTimer < 120) {
                if (handlingValue < 0)                    handlingValue = 0;
                handlingValue = handlingValue + 3;
                if (handlingValue > 100)                  handlingValue = 100;
            } else {
                //if(millis() % 2 == 0)
                handlingValue= 0;
                if (handlingValue < 0)                    handlingValue = 0;
            }
        }
    }

    PVector temp;
    if(!bKillEnabled)   throttleValue = (int)constrain(map(accelCount, -50, 50, -100, 100), -100, 100);
    else                throttleValue = 0;
    temp = new PVector(handlingValue, throttleValue);
    controlUI.update(temp);     // UI update
}

void serialEvent(Serial port) {
    if (port == feather) { 
        String inString = port.readStringUntil('\n'); 
    }
}


// 
void oscEvent(OscMessage incommingMessage) {
    if(incommingMessage.checkAddrPattern("/accel"))     tmpCount++;
}

void mouseMoved(){
    if(bMouseControlEnabled) handlingValue = (int)constrain(map(mouseX, 0, width, -100, 100), -100, 100); 
}