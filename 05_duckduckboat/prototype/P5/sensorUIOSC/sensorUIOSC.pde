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
VSlider whisperUI;           // whisper sensor           // A2
DigitalUI tearUI;           // tear sensor             // A0
DigitalUI handUI;
DigitalUI cutieUI;

int lastHeartRate = 0;
int fsrRate = 0;
int whisperRate = 0;

boolean bTearOn = false;
boolean bHandOn = false;
boolean bCupEmpty = false;
boolean bCutieOn = false;
float lastCutieOnTimer = 0;

PImage tearIcon;

boolean bGO = false;
boolean bBACK = false;
boolean bLEFT = false;;
boolean bRIGHT = false;

Serial feather;
Serial curie;

void setup() {
    size(900, 600);
    frameRate(60);

    oscP5 = new OscP5(this, 8000);
    
    for(int i=0; i<Serial.list().length; i++){
        println(Serial.list()[i]);
    }
    
    feather = new Serial(this, Serial.list()[2], 9600);
    feather.bufferUntil('\n');
    
    curie = new Serial(this, Serial.list()[1], 9600);
    curie.bufferUntil('\n');
    
    // SENSOR UI initilize
    heartRateUI = new VSlider("excitement", 0, 300, 250, 0);    // value min, value max, sliderWidth, sliderHeight
    fsrRateUI = new VSlider("patience", 3, 4095, 3850, 0);
    tearUI = new DigitalUI("tear", "tear.png");                   // indicator icon fileName
    handUI = new DigitalUI("together","handInHand.png");
    whisperUI = new VSlider("whisper", 30, 4095, 1200, 1);
    whisperUI.setLowThreshold(800);
    
    cutieUI = new DigitalUI("cutie","handInHand.png");
}

void draw() {
    
    // UI test
    //bTearOn = true;
    //bHandOn = true;
    
    //lastHeartRate = constrain((int)random(0, 300), 0, 300);
    //fsrRate = constrain((int)random(0, 4095), 0, 4095);
    //whisperRate = constrain((int)random(0, 4095), 0, 4095);
    //cutieRate= constrain((int)random(0, 100), 0, 100);
    
    background(0);    // disabled for fade effect

    // hearRate sensor
    // GO
    bGO = (heartRateUI.update(lastHeartRate) || fsrRateUI.update(fsrRate) || tearUI.update(bTearOn) || handUI.update(bHandOn));
    heartRateUI.draw(0, 0);

    // FSR sensor
    // GO
    //bGO = fsrRateUI.update(fsrRate);
    fsrRateUI.draw(300, 0);
    
    // tear sensor
    // GO
    //bGO = tearUI.update(bTearOn);
    tearUI.draw(600, 0);
    
    // whisper sensor
    // LEFT
    bLEFT = whisperUI.update(whisperRate);
    whisperUI.draw(0, 300);
    //println(whisperUI.update(whisperRate));
    
    // hand sensor
    // BACK
    //bLEFT = handUI.update(bHandOn);
    //handUI.draw(300, 300);
    
    // cutie sensor
    // RIGHT
    bRIGHT = cutieUI.update(bCutieOn);
    cutieUI.draw(600, 300);

    //// fadeout effect
    //pushStyle();
    //fill(0, 100);
    //rect(0, 0, width, height);
    //popStyle();
    
    if(millis() - lastCutieOnTimer > 200){
        bCutieOn = false;
    }
    
    if(bGO)       feather.write("f");
    if(bLEFT)     feather.write("l");
    if(bRIGHT)    feather.write("r");
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

    
    if (incommingMessage.checkAddrPattern("/demo/whisper")){
        whisperRate = incommingMessage.get(0).intValue();
    }
}


void keyReleased(){
    if(key == CODED){
        if(keyCode == UP)        feather.write("f");
        if(keyCode == DOWN)      feather.write("b");
        if(keyCode == LEFT)      feather.write("l");
        if(keyCode == RIGHT)     feather.write("r");
    }
}

void serialEvent(Serial port){
    if(port == feather){
        String inString = port.readStringUntil('\n'); 
        
        if(inString != null){
            inString = trim(inString);
            
            //println(inString);
            String [] items = split(inString, ',');
            
            if(items[0].equals("1"))     bCupEmpty = true;
            
            if(items[1].equals("1"))     bHandOn = true;
            else                         bHandOn = false;
        }
    } else if (port == curie){
        String inString = port.readStringUntil('\n');
        
        if(inString != null){
            //inString = trim(inString);
            
            //String [] items = split(inString, ',');
            //println(inString);
            
            if(inString.equals("C")){
                bCutieOn = true;
                lastCutieOnTimer = millis();
            }
        }
    }
}