float restartTimer;
float timerA, timerB;
boolean pressedA, pressedB;
float diff = 0;
int stageCounter;
boolean checkPass;

PFont stageFont;

void setup(){
    size(1280, 800);
    
    stageFont = loadFont("Hack-Bold-96.vlw");
    checkPass = false; 
    stageCounter = 1;
    restartTimer = 0;
    
    pressedA = pressedB = false;
}

void draw(){
    if(!pressedA && !pressedB){
        background(0);
    } else {
        if(diff < 100)    background(0, 255, 0);
        else              background(255, 0, 0);
    }
    
    fill(255, 255, 0);
    textFont(stageFont, 96);
    textAlign(CENTER);
    text(stageCounter, width/2, height/2);
    
    if(pressedA && pressedB){
        nextStage();
    }
}

boolean checkDiffTimer(){
    if(diff < 0.1)    return true;
    else              return false;
}

void nextStage(){
    pressedA = pressedB = false;
    stageCounter++;
}

void keyPressed(){
    if(key == 'z' || key == 'Z' ){
        timerA = millis();
        pressedA = true;
    }
    
    if(key == '/' || key == '?'){
        timerB = millis();
        pressedB = true;
    }
    
    diff = abs( timerA - timerB);
    
    
}