/*
    
 souceCode by GradeFreeEngineering team.
 
 
 [image License]
 teardrop icon is from "http://www.icon100.com/icon/196045/drop".
 free for cormmercial use. 
 
 handshake icon is from "https://thenounproject.com/term/handshake/24974/".
 by [Sam Garner](https://thenounproject.com/samtgarner)
 creative commons license.
 
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

float lastGoCmdTimer, lastBackCmdTimer, lastLeftCmdTimer, lastRightCmdTimer;



void setup() {
    size(300, 300);
    frameRate(60);

    controlUI = new TwoDimensionUI("controlUI", 300);    
    //cutieUI = new DigitalUI("cutie","handInHand.png");

    lastGoCmdTimer = lastBackCmdTimer = lastLeftCmdTimer = lastRightCmdTimer = millis();
}

void draw() {
    background(0);
    control();

    controlUI.draw(width/2, height/2);
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
        if (keyCode == LEFT) {
            // feather.write("l");
            lastLeftCmdTimer = millis();
        }   
        if (keyCode == RIGHT) {
            // feather.write("r");
            lastRightCmdTimer = millis();
        }
    }
}

void control() {
    if (lastGoCmdTimer > lastBackCmdTimer) {
        // go Command
        if (millis() - lastGoCmdTimer < 100) {
            if (throttleValue < 0)                    throttleValue = 0;
            // if (millis() % 2 == 0)
                                throttleValue++;
            if (throttleValue > 100)                  throttleValue = 100;
        } else {
            //if(millis() % == 0)
            throttleValue--;
            if (throttleValue<0)                      throttleValue = 0;
        }
    } else if (lastBackCmdTimer > lastGoCmdTimer) {
        // back Command
        if (millis() - lastBackCmdTimer < 100) {
            if (throttleValue > 0)                    throttleValue = 0;
            if (millis() % 2 == 0)                    throttleValue--;

            if (throttleValue<-100)                   throttleValue = -100;
        } else {
            //if(millis() % 1000 < 3) 
            throttleValue++;
            if (throttleValue > 0)                    throttleValue = 0;
        }
    }


    if (lastLeftCmdTimer > lastRightCmdTimer) {
        // left Command
        if (millis() - lastLeftCmdTimer < 100) {
            if (handlingValue > 0)                    handlingValue = 0;
            // if (millis()% 2 ==0)  
                                handlingValue--;
            if (handlingValue < -100)                 handlingValue = -100;
        } else {
            //if(millis() % 2 == 0)
            handlingValue++;
            if (handlingValue > 0)                    handlingValue = 0;
        }
    } else if (lastRightCmdTimer > lastLeftCmdTimer) {
        // right Command
        if (millis() - lastRightCmdTimer < 100) {
            if (handlingValue < 0)                    handlingValue = 0;
            if (millis() % 2 == 0)                    handlingValue++;
            if (handlingValue > 100)                  handlingValue = 100;
        } else {
            //if(millis() % 2 == 0)
            handlingValue--;
            if (handlingValue < 0)                    handlingValue = 0;
        }
    }

    PVector temp;
    temp = new PVector(handlingValue, throttleValue);
    //println(handlingValue + " : "  + throttleValue);
    //temp.x = throttleValue;
    //temp.y = handlingValue;
    controlUI.update(temp);
}



// void serialEvent(Serial port){
//     if(port == feather){
//         String inString = port.readStringUntil('\n'); 

//         if(inString != null){
//             inString = trim(inString);

//             //println(inString);
//             String [] items = split(inString, ',');

//             if(items[0].equals("1"))     bCupEmpty = true;

//             if(items[1].equals("1"))     bHandOn = true;
//             else                         bHandOn = false;
//         }
//     } 
//else if (port == curie){
//    String inString = port.readStringUntil('\n');

//    if(inString != null){
//        //inString = trim(inString);

//        //String [] items = split(inString, ',');
//        //println(inString);

//        if(inString.equals("C")){
//            bCutieOn = true;
//            lastCutieOnTimer = millis();
//        }
//    }
//}
// }