/*
    
 souceCode by GradeFreeEngineering team.
 
 
 [image License]
 teardrop icon is from "http://www.icon100.com/icon/196045/drop".
 free for cormmercial use. 
 
 handshake icon is from "https://thenounproject.com/term/handshake/24974/".
 by [Sam Garner](https://thenounproject.com/samtgarner)
 creative commons license.
 
 */
 
int SERVOMIN     = 205; // 4096 * (1/20)
int SERVOMIDDLE  = 308; // 4096 * (1.5 /20)
int SERVOMAX     = 410; // 4096 * (2 / 20)

import oscP5.*;
import netP5.*;
import processing.serial.*;

OscP5 oscP5;
NetAddress myRemoteLocation;

TwoDimensionUI controlUI;
Serial feather;

int throttleValue;  // -100 ~ 100
int handlingValue;  // -100 ~ 100

float lastGoCmdTimer, lastBackCmdTimer, lastLeftCmdTimer, lastRightCmdTimer, lastLORASentTimer;


void setup() {
    size(300, 300);
    frameRate(60);

    for(int i=0; i<Serial.list().length; i++){
        println("[" + i + "] : " + Serial.list()[i]);
    }
    controlUI = new TwoDimensionUI("controlUI", 300);    
    lastGoCmdTimer = lastBackCmdTimer = lastLeftCmdTimer = lastRightCmdTimer = lastLORASentTimer = millis();
    
    feather = new Serial(this, Serial.list()[1], 9600);
}

void draw() {
    background(0);
    control();

    controlUI.draw(width/2, height/2);
    
    if(millis() - lastLORASentTimer > 100){
        feather.write(throttleValue + 100);
        feather.write(handlingValue + 100);
        
        //println("THROTTLE Angle : " + int(map(throttleValue, -100, 100, SERVOMIN, SERVOMAX)) + "\t " + 
        //        "HANDING Angle :"   + int(map(handlingValue, -100, 100, SERVOMIN, SERVOMAX)));
        lastLORASentTimer = millis();
    }
}

void keyReleased() {
    if (key == CODED) {
        if (keyCode == UP) {
            // feather.write("f");
            lastGoCmdTimer = millis();
        }
        if (keyCode == DOWN) {
            // feather.write("b");
            lastBackCmdTimer = millis();
        }   
        //if (keyCode == LEFT) {
        //    // feather.write("l");
        //    lastLeftCmdTimer = millis();
        //}   
        //if (keyCode == RIGHT) {
        //    // feather.write("r");
        //    lastRightCmdTimer = millis();
        //}
    }
}

void keyPressed(){
    if(key == CODED){
        if(keyCode == LEFT){
            handlingValue--;
            if(handlingValue < -100)    handlingValue = -100;
        }
        
        if(keyCode == RIGHT){
            handlingValue++;
            if(handlingValue > 100)    handlingValue = 100;
        }
    }
}

void control() {
    if (lastGoCmdTimer > lastBackCmdTimer) {
        // go Command
        if (millis() - lastGoCmdTimer < 100) {
            //if (throttleValue < 0)                    throttleValue = 0;
            throttleValue++;
            if (throttleValue > 100)                  throttleValue = 100;
        } else {
            throttleValue--;
            if (throttleValue<0)                      throttleValue = 0;
        }
    } else if (lastBackCmdTimer > lastGoCmdTimer) {
        // back Command
        if (millis() - lastBackCmdTimer < 100) {
            //if (throttleValue > 0)                    throttleValue = 0;
            if (millis() % 2 == 0)                    throttleValue = throttleValue -2;
            if (throttleValue<-100)                   throttleValue = -100;
        } else {
            //if(millis() % 1000 < 3) 
            throttleValue++;
            if (throttleValue > 0)                    throttleValue = 0;
        }
    }


    // handling
    //if (lastLeftCmdTimer > lastRightCmdTimer) {
    //    // left Command
    //    if (millis() - lastLeftCmdTimer < 100) {
    //        if (handlingValue > 0)                    handlingValue = 0;
    //        // if (millis()% 2 ==0)  
    //                            handlingValue--;
    //        if (handlingValue < -100)                 handlingValue = -100;
    //    } else {
    //        //if(millis() % 2 == 0)
    //        handlingValue++;
    //        if (handlingValue > 0)                    handlingValue = 0;
    //    }
    //} else if (lastRightCmdTimer > lastLeftCmdTimer) {
    //    // right Command
    //    if (millis() - lastRightCmdTimer < 100) {
    //        if (handlingValue < 0)                    handlingValue = 0;
    //        if (millis() % 2 == 0)                    handlingValue++;
    //        if (handlingValue > 100)                  handlingValue = 100;
    //    } else {
    //        //if(millis() % 2 == 0)
    //        handlingValue--;
    //        if (handlingValue < 0)                    handlingValue = 0;
    //    }
    //}

    PVector temp;
    temp = new PVector(handlingValue, throttleValue);
    //println(handlingValue + " : "  + throttleValue);
    //temp.x = throttleValue;
    //temp.y = handlingValue;
    controlUI.update(temp);
}

void serialEvent(Serial port){
     if(port == feather){
         String inString = port.readStringUntil('\n'); 

         if(inString != null)    println(inString);
     } 
 }