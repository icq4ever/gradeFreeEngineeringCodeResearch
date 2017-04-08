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

OscP5 oscP5;
NetAddress myRemoteLocation;

// SENSOR UI
                                                       // PHOTON read pin no
VSlider heartRateUI;        // heartRate sensor        // D2
VSlider fsrRateUI;          // FSR sensor              // A1
VSlider wisperUI;           // wisper sensor           // A2
DigitalUI tearUI;           // tear sensor             // A0
DigitalUI handUI;



int lastHeartRate = 0;
int fsrRate = 0;
int wisperRate = 0;
boolean bTearOn = false;
boolean bHandOn = false;
PImage tearIcon;

void setup() {
    size(900, 600);
    frameRate(60);

    oscP5 = new OscP5(this, 8000);
    
    // SENSOR UI initilize
    heartRateUI = new VSlider("excitement", 0, 300, 250);    // value min, value max, sliderWidth, sliderHeight
    fsrRateUI = new VSlider("pain", 0, 4095, 4050);
    tearUI = new DigitalUI("tear", "tear.png");                   // indicator icon fileName
    handUI = new DigitalUI("together", "handInHand.png");
    wisperUI = new VSlider("whisper", 0, 4095, 4000);
}

void draw() {
    // UI test
    bTearOn = true;
    bHandOn = true;
    
    lastHeartRate = constrain((int)random(0, 300), 0, 300);
    fsrRate = constrain((int)random(0, 4095), 0, 4095);
    wisperRate = constrain((int)random(0, 4095), 0, 4095);;
    
    
    //background(0);    // disabled for fade effect

    // hearRate sensor
    heartRateUI.update(lastHeartRate);
    heartRateUI.draw(0, 0);

    // FSR sensor
    fsrRateUI.update(fsrRate);
    fsrRateUI.draw(600, 0);
    
    // tear sensor
    tearUI.update(bTearOn);
    tearUI.draw(300, 0);
    
    // hand sensor
    handUI.update(bHandOn);
    handUI.draw(0, 300);
    
    // wisper sensor
    wisperUI.update(wisperRate);
    wisperUI.draw(600, 300);

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