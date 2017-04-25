import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress target;

PFont font;

int lastSentTimer;

void setup(){
    size(100, 100);
    
    oscP5 = new OscP5(this, 9001);
    target = new NetAddress("192.168.0.255", 9000);
    
    font = loadFont("ShareTechMono-Regular-14.vlw");
    textFont(font);
}

void draw(){
    background(0);
    //for(int i=0; i<lastSentTimer.length; i++){
        if(millis() - lastSentTimer > 100){
            String oscAddress = "/accel";;
            OscMessage message = new OscMessage(oscAddress);
            
            //message.add(i+1);
            message.add(1);
            lastSentTimer = millis();
            message.add(lastSentTimer);
            
            oscP5.send(message, target);
        }
        
        
        text("/accel : \n" + lastSentTimer, 10, 40);
    //}
}