//enum LOVEDUCK_MODES {MODE_STANDBY, MODE_TRUST, MODE_CUTE, MODE_CRUSH, MODE_SYNC}
//enum SYNC_MODES {SUBMODE_STANDBY, SUBMODE_SYNCLEFT, SUBMODE_SYNCRIGHT, SUBMODE_SYNCDOWN, SUBMODE_SYNCJUMP}

//LOVEDUCK_MODES mode;

import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress p5Main, osculator;
PFont font;
String modeName[] = new String[5];
int numMode;

void setup(){
    size(300, 300);
    frameRate(60);
    
    oscP5= new OscP5(this, 8001);
    p5Main = new NetAddress("127.0.0.1", 8000);
    osculator = new NetAddress("127.0.0.1", 8002);
    
    font = loadFont("ShareTechMono-Regular-48.vlw");
    textFont(font);
    //mode = LOVEDUCK_MODES.MODE_STANDBY;
    
    modeName[0] = "/standby";
    modeName[1] = "/trust";
    modeName[2] = "/crush";
    modeName[3] = "/cute";
    modeName[4] = "/sync";
    
    numMode = 0;
}

void draw(){
    background(0);
    pushStyle();
    fill(#666666);
    text("p5Router", 30, 60);
    popStyle();
}

void keyPressed(){
    if(key == ','){
        OscMessage message= new OscMessage("/sync");
        message.add(1.0f);
        oscP5.send(message, p5Main);
        oscP5.send(message, osculator);
    }
    
    if(key == '.'){
        OscMessage message= new OscMessage("/standby");
        message.add(1.0f);
        oscP5.send(message, p5Main);
        oscP5.send(message, osculator);
    }
    
    if(key == ' '){
        OscMessage message= new OscMessage("/gofwd/touchosc");
        message.add(1.0f);
        oscP5.send(message, p5Main);
        oscP5.send(message, osculator);
    }
    
    
}

void keyReleased(){
    if(key == CODED){
        if(keyCode == RIGHT){
            numMode++;
            if(numMode > 4)    numMode = 0;
        }
        if(keyCode == LEFT){
            numMode--;
            if(numMode < 0)    numMode = 4;
        }
        
        OscMessage message= new OscMessage(modeName[numMode]);
        message.add(1.0f);
        oscP5.send(message, p5Main);
        
        println(numMode + " : "  + modeName[numMode]);
    }
}

void oscEvent(OscMessage incommingMessage){
    if(incommingMessage.checkAddrPattern("/standby"))    {
        float value = incommingMessage.get(0).floatValue();
        toP5Main("/standby", value);
        toOsculator("/standby", value);
    }
    if(incommingMessage.checkAddrPattern("/trust"))  {
        float value = incommingMessage.get(0).floatValue();
        toP5Main("/trust", value);
    }
    
    if(incommingMessage.checkAddrPattern("/cute")) {
        float value = incommingMessage.get(0).floatValue();
        toP5Main("/cute", value);
    }
    if(incommingMessage.checkAddrPattern("/crush")) {
        float value = incommingMessage.get(0).floatValue();
        toP5Main("/crush", value);
    }
    
    if(incommingMessage.checkAddrPattern("/sync"))       {
        float value = incommingMessage.get(0).floatValue();
        toOsculator("/sync", value);
        toP5Main("/sync", value);
    }
    
    if(incommingMessage.checkAddrPattern("/gofwd/touchosc")) {
        float value = incommingMessage.get(0).floatValue();
        toP5Main("/gofwd/touchosc", value);
    }
    if(incommingMessage.checkAddrPattern("/left")) {
        float value = incommingMessage.get(0).floatValue();
        toP5Main("/left", value);
    }
    if(incommingMessage.checkAddrPattern("/right")) {
        float value = incommingMessage.get(0).floatValue();
        toP5Main("/right", value);
    }
    if(incommingMessage.checkAddrPattern("/goback")) {
        float value = incommingMessage.get(0).floatValue();
        toP5Main("/goback", value);
    }
    
    if(incommingMessage.checkAddrPattern("/kill")) {
        float value = incommingMessage.get(0).floatValue();
        toP5Main("/kill", value);
    }
    
    if(incommingMessage.checkAddrPattern("/setmaxspeed")) {
        float value = incommingMessage.get(0).floatValue();
        toP5Main("/setmaxspeed", value);
    }
    
    background(255);
}

void toP5Main(String addr, float value){
    OscMessage message = new OscMessage(addr);
    message.add(value);
    oscP5.send(message, p5Main);
}

void toOsculator(String addr, float value){
    OscMessage message = new OscMessage(addr);
    message.add(value);
    oscP5.send(message, osculator);
}