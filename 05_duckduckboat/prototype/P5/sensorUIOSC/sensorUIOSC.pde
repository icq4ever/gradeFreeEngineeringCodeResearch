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

// SENSOR UI
                                                       // PHOTON read pin no
VSlider heartRateUI;        // heartRate sensor        // D2
VSlider fsrRateUI;          // FSR sensor              // A1
VSlider wisperUI;           // wisper sensor           // A2
DigitalUI tearUI;           // tear sensor             // A0
DigitalUI handUI;
VSlider cutieUI;

int lastHeartRate = 0;
int fsrRate = 0;
int wisperRate = 0;
int cutieRate = 0;
boolean bTearOn = false;
boolean bHandOn = false;
PImage tearIcon;

boolean bGO = false;
boolean bBACK = false;
boolean bLEFT = false;;
boolean bRIGHT = false;

Serial feather;

void setup() {
    size(900, 600);
    frameRate(60);

    oscP5 = new OscP5(this, 8000);
    
    feather = new Serial(this, Serial.list()[0], 9600);
    feather.bufferUntil('\n');
    
    // SENSOR UI initilize
    heartRateUI = new VSlider("excitement", 0, 300, 250);    // value min, value max, sliderWidth, sliderHeight
    fsrRateUI = new VSlider("patience", 0, 4095, 4050);
    tearUI = new DigitalUI("tear", "tear.png");                   // indicator icon fileName
    handUI = new DigitalUI("together", "handInHand.png");
    wisperUI = new VSlider("whisper", 0, 4095, 4000);
    cutieUI = new VSlider("cutie", 0, 100, 90);
}

void draw() {
    // UI test
    bTearOn = true;
    bHandOn = true;
    
    lastHeartRate = constrain((int)random(0, 300), 0, 300);
    fsrRate = constrain((int)random(0, 4095), 0, 4095);
    wisperRate = constrain((int)random(0, 4095), 0, 4095);
    cutieRate= constrain((int)random(0, 100), 0, 100);
    
    //background(0);    // disabled for fade effect

    // hearRate sensor
    // GO
    bGO = heartRateUI.update(lastHeartRate);
    heartRateUI.draw(0, 0);

    // FSR sensor
    // GO
    bGO = fsrRateUI.update(fsrRate);
    fsrRateUI.draw(300, 0);
    
    // tear sensor
    // GO
    bGO = tearUI.update(bTearOn);
    tearUI.draw(600, 0);
    
    // wisper sensor
    // LEFT
    bLEFT = wisperUI.update(wisperRate);
    wisperUI.draw(0, 300);
    
    // hand sensor
    // BACK
    bLEFT = handUI.update(bHandOn);
    handUI.draw(300, 300);
    
    // cutie sensor
    // RIGHT
    bRIGHT = cutieUI.update(cutieRate);
    cutieUI.draw(600, 300);

    // fadeout effect
    pushStyle();
    fill(0, 30);
    rect(0, 0, width, height);
    popStyle();
}

// get OSC message from particle PHOTON
void oscEvent(OscMessage incommingMessage) {
    if (incommingMessage.checkAddrPattern("/demo/heartRate")) {
        lastHeartRate = incommingMessage.get(0).intValue();
    }

    if (incommingMessage.checkAddrPattern("/demo/tear")) {
        if (incommingMessage.get(0).intValue() == 1)     bTearOn = true;
        else                                             bTearOn = false;
    }

    if (incommingMessage.checkAddrPattern("/demo/fsr")) {
        fsrRate = incommingMessage.get(0).intValue();
    }
    
    if (incommingMessage.checkAddrPattern("/demo/hand")){
        if (incommingMessage.get(0).intValue() == 1)    bHandOn = true;
        else                                            bHandOn = false;
    }
}


void keyPressed(){
    if(key == CODED){
        if(keyCode == UP)        feather.write("f");
        if(keyCode == DOWN)      feather.write("b");
        if(keyCode == LEFT)      feather.write("l");
        if(keyCode == RIGHT)     feather.write("r");
    }
}