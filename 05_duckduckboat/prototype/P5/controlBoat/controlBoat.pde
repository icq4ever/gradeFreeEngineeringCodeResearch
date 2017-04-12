import processing.serial.*;

Serial feather;

void setup(){
    feather = new Serial(this, Serial.list()[1], 9600);
    feather.bufferUntil('\n');
}

void draw(){
}

void keyReleased(){
    if(key == CODED){
        if(keyCode == UP)        feather.write("f");
        if(keyCode == DOWN)      feather.write("b");
        if(keyCode == LEFT)      feather.write("l");
        if(keyCode == RIGHT)     feather.write("r");
    }
}