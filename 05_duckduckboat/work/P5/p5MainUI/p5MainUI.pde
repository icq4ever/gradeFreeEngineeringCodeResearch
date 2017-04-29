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
PFont font, smallFont, titleFont;

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
String memberName[] = new String[5];
String modeName[] = new String[5];
boolean bMainRole[] = new boolean[5];
color backgroundColors[] = new color[5];
color bgColor = #000000;
color lastBGColor = #000000;
int lastBGChangedTimer;
int maxSpeed;
boolean bKilled = false;

// for debugging
boolean bDebugOn = true;
boolean bMainRoleDisplay = false;
String messageAddr = "";
float messageValue = 0;

void setup() {
    size(1600, 900);   
    frameRate(60);
    
    duckImage = loadImage("duck.png");
    waterImage = loadImage("water.png");
    
    font = loadFont("ShareTechMono-Regular-48.vlw");
    smallFont = loadFont("ShareTechMono-Regular-24.vlw");
    titleFont = loadFont("Seravek-Bold-120.vlw");
    
    maxSpeed = 50;
    
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
    
    controlUI = new TwoDimensionUI("controlUI", 400);    
    feather = new Serial(this, Serial.list()[1], 9600);
    
    tmpAccelCount = 0;
    tmpHandlingCount = 0;
    tmpGoBackCount = 0;
    
    for(int i=0; i<5; i++){
        bGotMessage[i] = false;
        lastGotFWDMessageTimer[i] = millis();
        bMainRole[i] = false;
    }   
    
    modeName[0] = "STANDBY";
    modeName[1] = "TRUST";
    modeName[2] = "CRUSH";
    modeName[3] = "CUTE";
    modeName[4] = "SYNC";
    
    memberName[0] = "kyuhan";
    memberName[1] = "hongchul";
    memberName[2] = "hojun";
    memberName[3] = "seyoon";
    memberName[4] = "yongjin";
    
    backgroundColors[0] = #000000;
    backgroundColors[1] = #7DFF15;
    backgroundColors[2] = #FF15C9;
    backgroundColors[3] = #FFD815;
    backgroundColors[4] = #15DEFF;
    
    lastBGChangedTimer = millis();
    
}

void draw() {
    if(millis() - lastBGChangedTimer > 300){
        lastBGColor = bgColor;
        bgColor = backgroundColors[duck_mode];
        lastBGChangedTimer = millis();
    }
    
    background(lerpColor(lastBGColor, bgColor, map(millis() - lastBGChangedTimer, 0, 200, 0.f, 1.f)));
    
    
    
    // mode display : deprecated due to set backgroundColor
    //pushStyle();
    //{
    //    textFont(titleFont);
    //    fill(#FFFF00);
    //    textAlign(CENTER, BOTTOM);
    //    text(modeName[duck_mode], width/2, 180);
    //}
    //popStyle();
    
    switch (duck_mode){
        case 0:    // standby
            throttleValue = 0;
            handlingValue = 0;
        break;
        
        case 1:    // trust
            int modeOneSpeed = (int)map(tilt_angle,0,90,0,80) + accelCount;
                
            throttleValue = (int)constrain(map(modeOneSpeed, -50, 50, -100, 100), -100, 100); 
            handlingValue = (int)constrain(map(handlingCount, -20, 20, -100, 100), -100, 100);
        break;
        
        case 2:    // crush
            // more throttle   
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
    
    if(throttleValue > maxSpeed)        throttleValue = maxSpeed;
    
    PVector temp = new PVector(handlingValue, throttleValue);
    controlUI.update(temp);     // UI update
    controlUI.draw(width/2, height-320);
    
    
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
    
    if(bKilled)    {
        throttleValue = 0;
        handlingValue = 0;
    }
    
    
    
    println(throttleValue + " : " + handlingValue );
    
    // send to LoRa
    if (millis() - lastLORASentTimer > 100) send2LoRA();

    if(bDebugOn)    debuggingPrint(messageAddr, messageValue);
    
    drawForwardMessageIndicator();
    //println(handlingValue + " : " + throttleValue);
}


void drawForwardMessageIndicator(){
    float roundSize = 180;
    float originHeight = 200;
    float alignDistance = 300;
    
    for(int i=0; i< lastGotFWDMessageTimer.length; i++){
        pushStyle();
        fill(#FFFFFF);
        textAlign(CENTER);
        textFont(smallFont);
        text(memberName[i], width/2 - (2-i)*alignDistance, originHeight-roundSize/2-40);
        popStyle();
        
        
        pushStyle();
        fill(#FF0000);
        noStroke();
        ellipse(width/2 - (2-i)*alignDistance, originHeight, roundSize, roundSize);
        popStyle();
        
        if(millis() - lastGotFWDMessageTimer[i] < 50){
            pushStyle();
            fill(#00FF00);
            ellipse(width/2 - (2-i)*alignDistance, originHeight, roundSize, roundSize);
            
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
		
        if(duck_mode == 2)    tmpAccelCount = tmpAccelCount + 2;
        else                  tmpAccelCount += messageValue;
        
        
        lastGotFWDMessageTimer[0] = millis();        
	}

	if(incommingMessage.checkAddrPattern("/gofwd/hc")){
		messageAddr = "/gofwd/hc"; 
		messageValue = incommingMessage.get(0).floatValue();
		if(duck_mode == 2)    tmpAccelCount = tmpAccelCount + 2;
        else                  tmpAccelCount += messageValue;
        lastGotFWDMessageTimer[1] = millis();
	}

	if(incommingMessage.checkAddrPattern("/gofwd/hj")){
		messageAddr = "/gofwd/hj"; 
		messageValue = incommingMessage.get(0).floatValue();
		if(duck_mode == 2)    tmpAccelCount = tmpAccelCount + 2;
        else                  tmpAccelCount += messageValue;
        lastGotFWDMessageTimer[2] = millis();
	}

	if(incommingMessage.checkAddrPattern("/gofwd/sy")){
		messageAddr = "/gofwd/sy"; 
		messageValue = incommingMessage.get(0).floatValue();
		if(duck_mode == 2)    tmpAccelCount = tmpAccelCount + 2;
        else                  tmpAccelCount += messageValue;
        
        lastGotFWDMessageTimer[3] = millis();
	}

	if(incommingMessage.checkAddrPattern("/gofwd/yj")){
		messageAddr = "/gofwd/yj"; 
		messageValue = incommingMessage.get(0).floatValue();
		if(duck_mode == 2)    tmpAccelCount = tmpAccelCount + 2;
        else                  tmpAccelCount += messageValue;
        
        if(duck_mode == 1)    tilt_angle = (int)messageValue;
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
        //if(duck_mode == 1)    tmpAccelCount = tmpAccelCount - 5;

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
    
    
    /* ========================================================== etc  ================= */
    if(incommingMessage.checkAddrPattern("/kill")) {
        messageAddr = "/kill";
        messageValue = incommingMessage.get(0).floatValue();
        //toP5Main("/kill", value);
        if(messageValue == 1.f)        bKilled = true;
        else                            bKilled = false;
    }
    
    if(incommingMessage.checkAddrPattern("/setmaxspeed")) {
        messageAddr = "/setmaxspeed";
        messageValue = incommingMessage.get(0).floatValue();
        maxSpeed = (int)messageValue;
        //toP5Main("/setmaxSpeed", value);
    }
}

void debuggingPrint(String addr, float value){
    pushStyle();
    fill(#000000);
    noStroke();
    rect(15, height/2-60, 500, 80); 
    popStyle();
    
    pushStyle();
    textFont(font);
    
    fill(#00FFFF);
    text("maxSpeed : " + maxSpeed, 30, height/2);
    //text(-10, 10, 40);
    popStyle();
    
    
    pushStyle();
    fill(#000000);
    noStroke();
    rect(15, height-180, 600, 150); 
    popStyle();
    
    pushStyle();
    textFont(font);
    
    fill(#FFFF00);
	text(addr, 30, height-120);
	fill(#00FF00);
	text(value, 30, height-60);
    //text(-10, 10, 40);
    
    popStyle();
}