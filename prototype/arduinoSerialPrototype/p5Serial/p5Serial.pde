import processing.serial.*;

Serial port;
float inByte = 0;

void setup(){
	size(400, 300);

	println(Serial.list());
	port = new Serial(this, Serial.list()[1], 115200);

	// don't denerate a serialEvent() unless get a newline character
	port.bufferUntil('\n');
}

void draw(){
	background(0);

	pushStyle();
	fill(255, 255, 0);
	text("A0 : " + inByte, 40, height/2);
	popStyle();
}

void serialEvent(Serial port){
	String inString = port.readStringUntil('\n');

	if(inString != null){
		// trim off any whitespace
		inString = trim(inString);

		// convert to an int and map 
		inByte = float(inString);
		// println(inByte);
	}
}