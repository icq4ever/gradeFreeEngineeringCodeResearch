/*
    
    souceCode by GradeFreeEngineering team.
    
    teardrop icon is from "http://www.icon100.com/icon/196045/drop".
    which is free for cormmercial use. 
*/

import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress myRemoteLocation;
VSlider heartRateUI;
PFont interfaceFont;

int lastHeartRate = 0;
boolean bTearOn = false;
PImage tearIcon;

void setup() {
    size(300, 300);
    frameRate(60);

    oscP5 = new OscP5(this, 8000);
    heartRateUI = new VSlider(0, 300, 50, 200);    // value min, value max, sliderWidth, sliderHeight 

    tearIcon = loadImage("tear.png");

    interfaceFont = loadFont("FiraSans-Regular-80.vlw");
    textFont(interfaceFont, 80);
}

void draw() {
    //background(0);    // disabled for fade effect
    
    heartRateUI.update(lastHeartRate);
    heartRateUI.draw(55, 50);
    
    textAlign(RIGHT, BOTTOM);
    fill(#FFFF00);
    if(lastHeartRate == 0){
        text("--", 260, 265);
    } else {
        text(lastHeartRate, 260, 265);
    }

    if(bTearOn)    image(tearIcon, 185, 60);
    
    
    // fadeout effect
    pushStyle();
    fill(0, 30);
    rect(0, 0, width, height);
    popStyle();
}

void oscEvent(OscMessage incommingMessage) {
    if (incommingMessage.checkAddrPattern("/demo/heartRate")) {
        lastHeartRate = incommingMessage.get(0).intValue();
    }
    
    if (incommingMessage.checkAddrPattern("/demo/tear")){
        if(incommingMessage.get(0).intValue() == 1)      bTearOn = true;
        else                                             bTearOn = false;
    }
}