import processing.serial.*;

Serial controlModule;

void setup(){
	controlModule = new Serial(this, Serial.list()[1], 115200);
}

void draw(){

}

void serialEvent(Serial p){
	//p.readString();
}