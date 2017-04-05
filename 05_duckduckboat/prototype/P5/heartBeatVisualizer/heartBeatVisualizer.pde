import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress myRemoteLocation;
VSlider heartRateUI;
PFont interfaceFont;

int lastHeartRate=0;

void setup() {
    size(300, 300);
    frameRate(60);

    oscP5 = new OscP5(this, 8000);
    heartRateUI = new VSlider(0, 120, 50, 200);
    heartRateUI.setValueMinMax(0, 300);

    //interfaceFont = loadFont("ShareTechMono-Regular-48.vlw");
    interfaceFont = loadFont("FiraSans-Regular-80.vlw");
    textFont(interfaceFont, 80);
}

void draw() {
    background(0);
    
    heartRateUI.update(lastHeartRate);
    heartRateUI.draw(55, 50);
    
    textAlign(RIGHT, BOTTOM);
    fill(#FFFF00);
    if(lastHeartRate == 0){
        text("--", 260, 265);
    } else {
        text(lastHeartRate, 260, 265);
    }

}

void oscEvent(OscMessage incommingMessage) {
    if (incommingMessage.checkAddrPattern("/heartRate/demo")== true) {
        lastHeartRate = incommingMessage.get(0).intValue();
    }
}