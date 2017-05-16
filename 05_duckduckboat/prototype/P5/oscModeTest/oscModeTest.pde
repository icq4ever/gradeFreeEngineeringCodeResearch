import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress remote;

int mode = 1;
PFont font;

void setup(){
    size(300, 300);
    frameRate(60);
    
    font = loadFont("Panton-BlackCaps-96.vlw");
    
    textFont(font);
    
    oscP5 = new OscP5(this, 8000);
    
    remote = new NetAddress("192.168.100.255", 8001);
}


void draw(){
    background(0);
    textAlign(CENTER);
    text(mode, width/2, height/2+32);
}

void keyReleased(){
    if(key == CODED){
        if (keyCode == UP){
            mode++;
            if(mode > 5)    mode = 1;
        }
        
        if(keyCode == DOWN){
            mode--;
            if(mode < 1)    mode = 5;
        }
    }
    
    OscMessage message = new OscMessage("/mode");
    message.add(mode);
    oscP5.send(message, remote);
}