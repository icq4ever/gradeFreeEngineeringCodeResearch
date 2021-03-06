import processing.serial.*;
import java.text.DecimalFormat;                 // 소수점 표기

float ROOM_TEMP = 20;

Serial feather;

boolean bStart = false;
boolean bMouseAnalysisOn = false;
float startTimer = 0;
int maxMIN = 12;
float MAX_TEMP=140;

float waterTemp;

// UIFont
PFont displayCheckFont;
PFont graphFont;
PFont smallFont;
PFont timerFont;

boolean buttonStatus[] = new boolean[10];
String buttonLabel[] = new String[10];

ButtonIndicator btnIndicator;

ArrayList <PVector> waterTempLog = new ArrayList<PVector>();

RaphagoActionList actionList;
MyunPulse myunPulse;

void setup(){
    // full dimensions across all of the attached screens with below command
    fullScreen(SPAN);   
    background(10); 
    // size(3840, 1080);

    // setup control module

    // detect OS 
    if(System.getProperty("os.name")== "Mac OS X"){
        feather  = new Serial(this, Serial.list()[1], 115200);  // osx
    } else {
        feather  = new Serial(this, Serial.list()[0], 115200);  // windows
    }
    feather.bufferUntil('\n');
    myunPulse = new MyunPulse(400, 100, 50);

    // load Font
    // displayCheckFont = loadFont("Hack-Bold-120.vlw");
    displayCheckFont = loadFont("SFUIDisplay-Semibold-24.vlw");
    graphFont = loadFont("SFUIDisplay-Medium-18.vlw");
    timerFont = loadFont("DejaVuSansMonoForPowerline-Bold-120.vlw");

    btnIndicator = new ButtonIndicator();
    actionList = new RaphagoActionList();    
}

void draw(){
    // background(10);

    // btnIndicator.update(buttonStatus);
    drawScreen1(0, 0);
    
    drawScreen2(1920, 0);
    btnIndicator.draw(buttonStatus, 1500, 80);  

    drawTimer(1400, 80); // draw Timer

    myunPulse.setPeriod(50);
    myunPulse.calcWave();
    myunPulse.renderWave(2000, 100, 3);
    if(bStart){
        actionList.update(millis() - startTimer);
    }
}


// draw ideal temp 
// maximum 6min
void drawScreen1(float _x, float _y){
    
    
    pushMatrix();
    pushStyle();
    translate(_x, _y);
    fill(10);
    rect(0, 0, 1920, 1080);
    
    textAlign(CENTER, CENTER);
    textFont(displayCheckFont);
    fill(255);
    // text("1", width/4, 600);
    drawScreenTitle("TEMPERATURE", #FFFF00);

    {
        pushMatrix();
        translate(80, 260);

        drawThermalAxis(1800, 800);
        // drawIdealThermalGraph(1840, 960, 40);        // THERMAL GRAPH
        drawRealThermalGraph(1800, 800);
        // fill(255, 0, 0);
        // rect(0, 0, 1840, 100);

        popMatrix();
    }

    popStyle();
    popMatrix();
}


// 
// NHR (Noodle Heart Rate)
void drawScreen2(float _x, float _y){
    pushMatrix();
    pushStyle();
    translate(_x, _y);
    fill(10);
    rect(0, 0, 1920, 0);
    textAlign(CENTER, CENTER);
    textFont(displayCheckFont);
    fill(255);
    text("2", width/4, 540);

    drawScreenTitle("NOODLE HEART RATE", #FFFF00);

    popStyle();
    popMatrix();
}


void drawScreenTitle(String _title, color _c){
    pushStyle();
    textFont(displayCheckFont);
    // textFont(graphFont);
    fill(_c);
    textAlign(LEFT, TOP);
    text(_title, 40, 40);
    popStyle();
}

void drawThermalAxis(float _width, float _height){
    
    float graphWidth = _width;
    float graphHeight = _height;

    pushMatrix();
    pushStyle();
    stroke(#FFFFFF);
    strokeWeight(2);
    line(0, graphHeight, graphWidth, graphHeight);
    line(0, graphHeight, 0, 0);

    // 6MIN : 6 *12 + 1
    // 15MIN : 15*12+1
    // time Dimension (X)
    for(int i=0; i<(maxMIN*12+1); i++){
        if(i%12 == 0)    {
            strokeWeight(2);
            line(i*graphWidth/(maxMIN*12), _height-20, i*graphWidth/(maxMIN*12), _height+20);

            // draw minute legend
            pushStyle();
            textFont(graphFont);
            textAlign(LEFT, TOP);
            fill(#FFFFFF);
            noStroke();
            text(pixel2Second(i*graphWidth/(maxMIN*12), graphWidth) + "Min" , i*graphWidth/(maxMIN*12)-10, _height+30);
            popStyle();
        } else {
            strokeWeight(1);
            line(i*graphWidth/(maxMIN*12), _height-10, (i)*graphWidth/(maxMIN*12), _height+10);
        }     
    }

    // thermal degree (Y_)
    int detailLevel = 14;
    for(int i=0; i<detailLevel*5+1; i++){
        if(i%5 == 0)  {
            
            strokeWeight(2);
            line(-10, graphHeight-i*graphHeight/(detailLevel*5), 10, graphHeight-i*graphHeight/(detailLevel*5));
            
            pushStyle();
            textAlign(RIGHT, CENTER);
            fill(#FFFFFF);
            noStroke();
            textFont(graphFont);
            text(pixel2Celcius(i*graphHeight/(detailLevel*5)+1, graphHeight) + "°C", -15, graphHeight-i*graphHeight/(detailLevel*5)-1);
            popStyle();
        }
    }
    
    popStyle();


    // mouse analysis
    if(bMouseAnalysisOn && mouseY < 1060 && mouseX > 8 && mouseX < 1920){
        pushStyle();

        noFill();
        stroke(255);
        line(0, mouseY-260, mouseX-80, mouseY-260);
        line(mouseX-80, mouseY-260, mouseX-80, 800);

        fill(#FF0000);
        textSize(18);
        // noStorke();
        // textAlign(LEFT, MIDDLE);
        popStyle();


    }
    popMatrix();
}

void drawIdealThermalGraph(float _width, float _height, float _margin){
    float graphWidth = _width-_margin*2;
    // draw x,y line
    pushMatrix();
    translate(_margin, _margin);
    pushStyle();
    stroke(#FFFF00);
    strokeWeight(4);
    for(int i=0; i<_width-_margin*2; i++){
        line(i, _height-_margin*2-log(i)*50, i+1, _height-_margin*2-log(i+1)*50);
    }

    popStyle();
    popMatrix();
}

void drawTimer(float _x, float _y){
    pushStyle();
    pushMatrix();
    translate(_x, _y);
    textFont(timerFont);
    textAlign(RIGHT, TOP);
    textSize(80);
    fill(#FFFF00);
    if(!bStart) {
        text("STANDBY", 0, 0);
    } else {
        text(millisToString(millis() - startTimer), 0, 0);
    }

    popMatrix();
    popStyle();
}


// ======================================================================== CONVERT UTILITY =====
int pixel2Second(float _pixelLength, float _pixelWidth){
    return (int)map(_pixelLength, 0, _pixelWidth, 0, maxMIN*600)/600;
}

int pixel2Celcius(float _pixelLength, float _pixelHeight){
    return (int)map(_pixelLength, 0, _pixelHeight, 0, MAX_TEMP);
}

float milliSec2Pixel(float _millis, float _pixelWidth){
    return map(_millis, 0, maxMIN*60*1000, 0, _pixelWidth);
}

float celcius2Pixel(float _temp, float _pixelHeight){
    return map(_temp, 0, MAX_TEMP, 0, _pixelHeight);
}

// convert millis to string(min:sec:millis)
String millisToString(float _millis){
    int mil = (int)_millis%1000;
    int sec = ((int)(_millis)/1000) % 60;
    int min = ((int)(_millis)/60000) % 60;
    
    String tMil, tSec, tMin;

    if(mil < 10)        {tMil = "00" + Integer.toString(mil);}
    else if(mil < 100)  {tMil = "0"  + Integer.toString(mil);}
    else                {tMil = Integer.toString(mil);}

    if(sec < 10)    tSec = "0" + Integer.toString(sec);
    else            tSec = Integer.toString(sec);

    tMin = Integer.toString(min);

    return (tMin+ ":" + tSec + "." + tMil);
}

// =============================================================================================


void drawRealThermalGraph(float _width, float _height){
    if(!bStart){ return;}
    else {
        DecimalFormat formatDegree = new DecimalFormat("#.##");

        // waterTemp
        pushStyle();
        stroke(#00FFFF);
        strokeWeight(3);
        if(waterTempLog.size() >1){
            for(int i=1; i<waterTempLog.size(); i++){
                if(i == 1)  line(0, 800-celcius2Pixel(0, 800), milliSec2Pixel(waterTempLog.get(i).x, 1800), 800- celcius2Pixel(waterTempLog.get(i).y, 800));
                else        line(milliSec2Pixel(waterTempLog.get(i-1).x, 1800), 800- celcius2Pixel(waterTempLog.get(i-1).y, 800), milliSec2Pixel(waterTempLog.get(i).x, 1800), 800- celcius2Pixel(waterTempLog.get(i).y, 800));
            }
        }
        textFont(displayCheckFont);
        textSize(18);
        fill(#00FFFF);
        noStroke();
        textAlign(LEFT, BOTTOM);
        text(formatDegree.format(waterTemp) + " °C", milliSec2Pixel(millis()-startTimer, 1800), 800 - celcius2Pixel(waterTemp, 800) + 20);

        noFill();
        strokeWeight(1);
        stroke(#E0E0E0);
        if(waterTempLog.size() > 1){
            line( milliSec2Pixel(millis()-startTimer,1800), 800, 
                  milliSec2Pixel(millis()-startTimer,1800), 800 - celcius2Pixel(waterTempLog.get(waterTempLog.size()-1).y, 800)
                );
            line(0, 800 - celcius2Pixel(waterTempLog.get(waterTempLog.size()-1).y, 800),
                milliSec2Pixel(millis()-startTimer,1800), 800 - celcius2Pixel(waterTempLog.get(waterTempLog.size()-1).y, 800)
                );
        }
        popStyle();

    }


}

void serialEvent(Serial port){
    String inString = port.readStringUntil('\n');
    
    // println(inString);

    if(inString != null){
        inString = trim(inString);

        String [] items = split(inString, ',');

        // println(items[0]);

        for(int i=0; i<10; i++){
            if(items[i].equals("1"))     {
                buttonStatus[i] = true;
                // print("okok");
            } else {      
                buttonStatus[i] = false;
            }
        }

        waterTemp = Float.parseFloat(items[10]);
        

        if(bStart) {
            
            PVector _waterTemp = new PVector(millis() - startTimer, waterTemp);
            
            waterTempLog.add(_waterTemp);
            // waterTempLog.add(_waterTemp);
        } 
    }

    if(buttonStatus[7] && !bStart)  {
        bStart = true;
        startTimer = millis();
        actionList.activate(); 
    }
}

void keyReleased(){
    if(key == ' ' ) {
        if(bStart)  {
            bStart = false;

            waterTempLog.clear();
            actionList.deactivate();
            //waterTempLog.clear();
        }
    }
}

void mousePressed(){
    // bMouseAnalysisOn = !bMouseAnalysisOn;
}