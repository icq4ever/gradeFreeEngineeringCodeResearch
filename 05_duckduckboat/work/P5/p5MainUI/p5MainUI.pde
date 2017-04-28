/*
    
 souceCode by GradeFreeEngineering team.
 
 
 [image License]
 duck pixel icon is from "http://piq.codeus.net/picture/370697/rubber_ducky"
 by ZacharyB
 free for cormmercial use. 

 Platformer Art Deluxe (Pixel Redux) by Kenney Vleugels (www.kenney.nl)
 License (CC0)

 TODO :
 [ ] : add to feather (!while) in serial.

 */
 
 
 // [ ] : set maxvalue
 // 
 


import controlP5.*;
import oscP5.*;
import netP5.*;
import processing.serial.*;

OscP5 oscP5;
NetAddress photons;

TwoDimensionUI controlUI;
Serial feather;

PImage duckImage, waterImage;

int throttleValue;  // -100 ~ 100
int handlingValue;  // -100 ~ 100
PFont font, smallFont;

int lastLORASentTimer;
int lastLRCmdCheckedTimer;

int accelCountBuffer[] = new int[10];
int handlingCountBuffer[] = new int[10];
int accelCount, handlingCount;
int tmpAccelCount, tmpHandlingCount;
int tmpGoBackCount;
int lastMessageCountCheckedTimer;
int bufferIndex = 0;

int tilt_angle = 0;
int duck_mode = 0;

// member osc status
boolean bGotMessage[] = new boolean[5];
int lastGotFWDMessageTimer[] = new int[5];

// for debugging
boolean bDebugOn = false;
String messageAddr = "";
float messageValue = 0;

void setup() {
    size(1600, 900);   
    frameRate(60);
    
    duckImage = loadImage("duck.png");
    waterImage = loadImage("water.png");
    
    font = loadFont("ShareTechMono-Regular-48.vlw");
    smallFont = createFont("arial", 14);
    
    
    oscP5 = new OscP5(this, 8000);
    photons = new NetAddress("192.168.100.255", 8001);
    
    for (int i=0; i<Serial.list().length; i++) {
        println("[" + i + "] : " + Serial.list()[i]);
    }
    
    lastMessageCountCheckedTimer = lastLORASentTimer = millis();
    
    for(int i=0; i<accelCountBuffer.length; i++)    {
        accelCountBuffer[i] = 0;
        handlingCountBuffer[i] = 0;
        
    }
    
    controlUI = new TwoDimensionUI("controlUI", 300);    
    feather = new Serial(this, Serial.list()[1], 9600);
    
    tmpAccelCount = 0;
    tmpHandlingCount = 0;
    tmpGoBackCount = 0;
    
    for(int i=0; i<5; i++){
        bGotMessage[i] = false;
        lastGotFWDMessageTimer[i] = millis();
    }   
}

void draw() {
    background(0);
    
    switch (duck_mode){
        case 0:    // standby
            throttleValue = 0;
            handlingValue = 0;
        break;
        
        case 1:    // trust
            accelCount = (int)map(tilt_angle,0,90,0,40);
            throttleValue = (int)constrain(map(accelCount, -50, 50, -100, 100), -100, 100);
            handlingValue = (int)constrain(map(handlingCount, -20, 20, -100, 100), -100, 100);
        break;
        
        case 2:    // crush
            throttleValue = (int)constrain(map(accelCount, -50, 50, -100, 100), -100, 100);
            handlingValue = (int)constrain(map(handlingCount, -20, 20, -100, 100), -100, 100);
        break;
        
        case 3:    // cute
            throttleValue = (int)constrain(map(accelCount, -50, 50, -100, 100), -100, 100);
            handlingValue = (int)constrain(map(handlingCount, -20, 20, -100, 100), -100, 100);
        break;
        
        case 4:    // sync
            throttleValue = (int)constrain(map(accelCount, -50, 50, -100, 100), -100, 100);
            handlingValue = (int)constrain(map(handlingCount, -20, 20, -100, 100), -100, 100);
        break;    
    }
    
    PVector temp = new PVector(handlingValue, throttleValue);
    controlUI.update(temp);     // UI update
    controlUI.draw(width/2, height/2);
    
    
    accelCount = handlingCount = 0;
    
    // fill buffer
    if(millis() - lastMessageCountCheckedTimer > 50){
        accelCountBuffer[bufferIndex] = tmpAccelCount;
        handlingCountBuffer[bufferIndex] = tmpHandlingCount;

        tmpAccelCount = tmpHandlingCount = 0;
        bufferIndex++;

        if(bufferIndex >= accelCountBuffer.length)  bufferIndex = 0;
        lastMessageCountCheckedTimer = millis();
    }

    // calc
    for(int i=0; i<accelCountBuffer.length; i++){
        accelCount += accelCountBuffer[i];
        handlingCount  += handlingCountBuffer[i];
    }
    
    if(millis() - lastLRCmdCheckedTimer > 100)       handlingValue = 0;
    
    // send to LoRa
    if (millis() - lastLORASentTimer > 100) send2LoRA();

    if(bDebugOn)    debuggingPrint(messageAddr, messageValue);
    
    drawForwardMessageIndicator();
    //println(handlingValue + " : " + throttleValue);
}


void drawForwardMessageIndicator(){
    for(int i=0; i< lastGotFWDMessageTimer.length; i++){
        pushStyle();
        fill(#FF0000);
        noStroke();
        ellipse(width/2 - (i-2)*200, 100, 100, 100);
        popStyle();
        
        if(millis() - lastGotFWDMessageTimer[i] < 50){
            pushStyle();
            fill(#00FF00);
            ellipse(width/2 - (i-2)*200, 100, 100, 100);
            
            popStyle();
        }
        
        
    }
}

void keyPressed(){
    if(key == 'd' || key == 'D'){
        bDebugOn = !bDebugOn;
    }
}


void send2LoRA(){
    byte message[] = new byte[4];
    
    message[0] = '/';
    message[1] = (byte)(throttleValue + 100);
    message[2] = (byte)(handlingValue + 100);
    
    feather.write(message);
    lastLORASentTimer = millis();
}


void serialEvent(Serial port) {
    if (port == feather) {
        // TODO : 
        // do something
        //String inString = port.readStringUntil('\n'); 
    }
}


// check oscMessage 
void oscEvent(OscMessage incommingMessage) {    
	/* ========================================================== /gofwd ================= */
	if(incommingMessage.checkAddrPattern("/gofwd/kh")){
		messageAddr = "/gofwd/kh"; 
		messageValue = incommingMessage.get(0).floatValue();
		tmpAccelCount += messageValue;
        lastGotFWDMessageTimer[0] = millis();        
	}

	if(incommingMessage.checkAddrPattern("/gofwd/hc")){
		messageAddr = "/gofwd/hc"; 
		messageValue = incommingMessage.get(0).floatValue();
		tmpAccelCount += messageValue;
        lastGotFWDMessageTimer[1] = millis();
	}

	if(incommingMessage.checkAddrPattern("/gofwd/hj")){
		messageAddr = "/gofwd/hj"; 
		messageValue = incommingMessage.get(0).floatValue();
		tmpAccelCount += messageValue;
        lastGotFWDMessageTimer[2] = millis();
	}

	if(incommingMessage.checkAddrPattern("/gofwd/sy")){
		messageAddr = "/gofwd/sy"; 
		messageValue = incommingMessage.get(0).floatValue();
		tmpAccelCount += messageValue;
        lastGotFWDMessageTimer[3] = millis();
	}

	if(incommingMessage.checkAddrPattern("/gofwd/yj")){
		messageAddr = "/gofwd/yj"; 
		messageValue = incommingMessage.get(0).floatValue();
		tmpAccelCount += messageValue;
        lastGotFWDMessageTimer[4] = millis();
	}

    if(incommingMessage.checkAddrPattern("/gofwd/touchosc")) {
        messageAddr = "/gofwd/touchosc"; 
        messageValue = incommingMessage.get(0).floatValue();
        tmpAccelCount = tmpAccelCount + 2;
    }


    if(incommingMessage.checkAddrPattern("/left")) {
		messageAddr = "/left"; 
		messageValue = incommingMessage.get(0).floatValue();
		tmpHandlingCount--;
		lastLRCmdCheckedTimer = millis();
    }

    if(incommingMessage.checkAddrPattern("/right"))  {
		messageAddr = "/right"; 
		messageValue = incommingMessage.get(0).floatValue();
		tmpHandlingCount++;
		lastLRCmdCheckedTimer = millis();
    }
    
    if(incommingMessage.checkAddrPattern("/goback"))    {
		messageAddr = "/goback"; 
		messageValue = incommingMessage.get(0).floatValue();
		tmpAccelCount=tmpAccelCount - 4;
	}

    
    /* ========================================================== mode ================= */
	if(incommingMessage.checkAddrPattern("/standby"))	{
		messageAddr = "/standby"; 
		messageValue = incommingMessage.get(0).floatValue();
		duck_mode = 0;
	}

    if(incommingMessage.checkAddrPattern("/trust"))	{
		messageAddr = "/trust"; 
		messageValue = incommingMessage.get(0).floatValue();
		duck_mode = 1;
	}

    if(incommingMessage.checkAddrPattern("/crush"))    {
        messageAddr = "/crush"; 
        messageValue = incommingMessage.get(0).floatValue();
        duck_mode = 2;
    }
    
    if(incommingMessage.checkAddrPattern("/cute")) {
        float value = incommingMessage.get(0).floatValue();
        messageAddr = "/cute"; 
        messageValue = incommingMessage.get(0).floatValue();
        duck_mode = 3;
    }
    
    if(incommingMessage.checkAddrPattern("/sync"))    {
        messageAddr = "/sync"; 
        messageValue = incommingMessage.get(0).floatValue();
        duck_mode = 4;
    }
}

void debuggingPrint(String addr, float value){
    pushStyle();
    textFont(font);
    
    fill(#FFFF00);
	text(addr, 30, height-120);
	fill(#00FF00);
	text(value, 30, height-60);
    //text(-10, 10, 40);
    
    popStyle();
}