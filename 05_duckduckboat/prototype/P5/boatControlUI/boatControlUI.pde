/*
    
 souceCode by GradeFreeEngineering team.
 
 
 [image License]
 teardrop icon is from "http://www.icon100.com/icon/196045/drop".
 free for cormmercial use. 
 
 handshake icon is from "https://thenounproject.com/term/handshake/24974/".
 by [Sam Garner](https://thenounproject.com/samtgarner)
 creative commons license.
 
 */

/*
 /duck/accel : 1
 /duck/gyro
 /duck/whisper
 */

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
boolean bMouseControlEnabled = false;
boolean bKillEnabled = false;



PFont font;

void setup() {
    size(300,300);
    frameRate(60);
    
    font = loadFont("ShareTechMono-Regular-24.vlw");
    oscP5 = new OscP5(this, 8000);

    for (int i=0; i<Serial.list().length; i++) {
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

    if (millis() - lastLORASentTimer > 100) {
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
    if(key == ' ' ){
        bMouseControlEnabled = !bMouseControlEnabled;
        if(!bMouseControlEnabled)    handlingValue = 0;
    }
    
    if(key == 'z' || key == 'Z'){
        bKillEnabled = !bKillEnabled;
        if(!bKillEnabled){
            throttleValue = 0;
            handlingValue = 0;
        }
        
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


void keyPressed() {
    if (key == CODED) {
        if(keyCode == UP){
            throttleValue++;
            if(throttleValue > 100)    throttleValue = 100;
        }
        
        if(keyCode == DOWN){
            throttleValue--;
            if(throttleValue < -100)   throttleValue = -100;
        }
        
        
        if (keyCode == LEFT) {
            handlingValue--;
            if (handlingValue < -100)    handlingValue = -100;
        }

        if (keyCode == RIGHT) {
            handlingValue++;
            if (handlingValue > 100)    handlingValue = 100;
        }
    }
}

void control() {
    if (lastGoCmdTimer > lastBackCmdTimer) {
        // go Command
        if (millis() - lastGoCmdTimer < 150) {
            throttleValue++;
            if (throttleValue > 100)                  throttleValue = 100;
        } else {
            throttleValue = 0;
            if (throttleValue<0)                      throttleValue = 0;
        }
    } else if (lastBackCmdTimer > lastGoCmdTimer) {
        // back Command
        if (millis() - lastBackCmdTimer < 100) {
            if (millis() % 2 == 0)                    throttleValue--;
            if (throttleValue<-100)                   throttleValue = -100;
        } else {
            //if(millis() % 1000 < 3) 
            throttleValue++;
            if (throttleValue > 0)                    throttleValue = 0;
        }
    }


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
    temp = new PVector(handlingValue, throttleValue);
    //println(handlingValue + " : "  + throttleValue);
    //temp.x = throttleValue;
    //temp.y = handlingValue;
    controlUI.update(temp);
}

void serialEvent(Serial port) {
    if (port == feather) { 
        String inString = port.readStringUntil('\n'); 

        //if(inString != null)    println(inString);
    }
}

void oscEvent(OscMessage incommingMessage) {
    if (incommingMessage.checkAddrPattern("/duck/accel")) {
        lastGoCmdTimer = millis();
    } else {
        //lastBackCmdTimer = millis();
        //if (throttleValue<0)                      throttleValue = 0;
    }
    
    if (incommingMessage.checkAddrPattern("/duck/whisper")) {
        if(!bMouseControlEnabled)    lastRightCmdTimer = millis();
    }
    
    if (incommingMessage.checkAddrPattern("/duck/piezo")) {
        if(!bMouseControlEnabled)    lastLeftCmdTimer = millis();
    }
}

void mouseMoved(){
    if(bMouseControlEnabled) handlingValue = (int)constrain(map(mouseX, 0, width, -100, 100), -100, 100); 
}