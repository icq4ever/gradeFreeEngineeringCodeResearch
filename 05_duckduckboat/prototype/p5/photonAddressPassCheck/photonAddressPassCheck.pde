import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress remote;

void setup(){
    size(300, 300);
    
    oscP5= new OscP5(this, 8000);
    remote = new NetAddress("127.0.0.1", 8002);
}

void draw(){
    background(0);
}

void mousePressed(){
    OscMessage message= new OscMessage("/sync");
    message.add(1);
    oscP5.send(message, remote);
    
    background(255);
}


/*
8001번 포트 
*/