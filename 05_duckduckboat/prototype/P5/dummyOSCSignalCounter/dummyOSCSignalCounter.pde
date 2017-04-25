import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress target;

PFont font;
int accelCount;

int accelCountBuffer[] = new int[3];
int bufferIndex=0;
int lastMessageCountCheckedTimer;


void setup(){
    size(300, 100);
    
    oscP5 = new OscP5(this, 9000);
    target = new NetAddress("192.168.0.255", 9001);
    
    font = loadFont("ShareTechMono-Regular-14.vlw");
    textFont(font);
    
    for(int i=0; i<accelCountBuffer.length; i++){
        accelCountBuffer[i] = 0;
    }
    lastMessageCountCheckedTimer = millis();
}

void draw(){
    background(0);
    
    text(accelCount, 20, 20);
    
    //textAlign(CENTER, CENTER);
    //text(accelCount, width/2, height/2);
    
    rect(0, height-20, map(accelCount, 0, 100, 0, width), 20);
    
    if(millis() - lastMessageCountCheckedTimer > 1000){
        accelCount = accelCountBuffer[bufferIndex];
        accelCountBuffer[bufferIndex] = 0;
        bufferIndex++;
        if(bufferIndex >= accelCountBuffer.length)    bufferIndex = 0;
        lastMessageCountCheckedTimer = millis();
    }
}

void oscEvent(OscMessage _message){
    if(_message.checkAddrPattern("/accel"))        accelCountBuffer[bufferIndex]++;    
}