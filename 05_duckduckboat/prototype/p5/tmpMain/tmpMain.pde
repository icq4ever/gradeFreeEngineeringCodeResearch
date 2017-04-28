//enum LOVEDUCK_MODES {MODE_STANDBY, MODE_TRUST, MODE_CUTE, MODE_CRUSH, MODE_SYNC}
//enum SYNC_MODES {SUBMODE_STANDBY, SUBMODE_SYNCLEFT, SUBMODE_SYNCRIGHT, SUBMODE_SYNCDOWN, SUBMODE_SYNCJUMP}

//LOVEDUCK_MODES mode;

import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress p5Main, osculator;
PFont font;

String messageString = "/";
float pValue;

void setup(){
    size(600, 300);
    frameRate(60);
    
    oscP5= new OscP5(this, 8000);
    osculator = new NetAddress("127.0.0.1", 8002);
    font = loadFont("ShareTechMono-Regular-48.vlw");
    textFont(font);
}

void draw(){
    background(0);
    
    pushStyle();
    fill(#FFFF00);
    text("p5MAIN", 30, 60);
    popStyle();
    
    text(messageString, 30, height/2);
    fill(#00FF00);
    text(pValue, 30, height/2+45); 
}



void oscEvent(OscMessage incommingMessage){
    
    if(incommingMessage.checkAddrPattern("/sync"))  {
        float value = incommingMessage.get(0).floatValue();
        messageString = "/sync";
        pValue = value;
    }
    
    if(incommingMessage.checkAddrPattern("/gofwd/touchosc")) {
        float value = incommingMessage.get(0).floatValue();
        messageString = "/gofwd/touchosc";
        pValue = value;
    }
    
    if(incommingMessage.checkAddrPattern("/left")) {
        float value = incommingMessage.get(0).floatValue();
        messageString = "/left";
        pValue = value;
    }
    
    if(incommingMessage.checkAddrPattern("/right"))   {
        float value = incommingMessage.get(0).floatValue();
        messageString = "/right";
        pValue = value;
    }
    
    if(incommingMessage.checkAddrPattern("/goback")) {
        float value = incommingMessage.get(0).floatValue();
        messageString = "/goback";
        pValue = value;
    }
    
    if(incommingMessage.checkAddrPattern("/standby"))  {
        float value = incommingMessage.get(0).floatValue();
        messageString = "/standby";
        pValue = value;
    }
    
    if(incommingMessage.checkAddrPattern("/trust")) {
        float value = incommingMessage.get(0).floatValue();
        messageString = "/trust";
        pValue = value;
    }
    
    if(incommingMessage.checkAddrPattern("/cute")) {
        float value = incommingMessage.get(0).floatValue();
        messageString = "/cute";
        pValue = value;
    }
    
    if(incommingMessage.checkAddrPattern("/crush")) {
        float value = incommingMessage.get(0).floatValue();
        messageString = "/crush";
        pValue = value;
    }
    
    
    
}