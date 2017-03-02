import processing.serial.*;

float ROOM_TEMP = 20;

Serial feather;

boolean bStart = false;
float startTimer = 0;
float MAX_TEMP=120;

// UIFont
PFont displayCheckFont;
PFont graphFont;

void setup(){
	// size(1920, 1080);

	// full dimensions across all of the attached screens with below command
	fullScreen(P3D, SPAN);	
	// size(3840, 1080);

	// setup control module
	feather	= new Serial(this, Serial.list()[1], 115200);
	feather.bufferUntil('\n');

	// load Font
	// displayCheckFont = loadFont("Hack-Bold-120.vlw");
	displayCheckFont = loadFont("SFUIDisplay-Semibold-24.vlw");
	graphFont = loadFont("SFUIDisplay-Medium-18.vlw");
}

void draw(){
	background(10);

	drawScreen1(0, 0);
	drawScreen2(1920, 0);
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
	text("1", width/4, 600);
	drawScreenTitle("TEMPERATURE", #FFFF00);


	pushMatrix();
	translate(40, 200);

	drawThermalAxis(1840, 960, 40);
	drawIdealThermalGraph(1840, 960, 40);
	drawRealThermalGraph(1840, 960, 40);
	// fill(255, 0, 0);
	// rect(0, 0, 1840, 100);

	popMatrix();


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

void drawThermalAxis(float _width, float _height, float _margin){
	float graphWidth = _width-_margin*2;
	float graphHeight = _height-_margin*2;

	pushMatrix();
	translate(_margin, _margin);
	pushStyle();
	stroke(#FFFFFF);
	strokeWeight(2);
	line(0, graphHeight, graphWidth, graphHeight);
	line(0, graphHeight, 0, 0);


	// time Dimension (X)
	for(int i=0; i<73; i++){
		if(i%12 == 0)	{
			strokeWeight(2);
			line(i*graphWidth/72, _height-60, i*graphWidth/72, _height-100);

			// draw minute legend
			pushStyle();
			textFont(graphFont);
			textAlign(LEFT, TOP);
			fill(#FFFFFF);
			noStroke();
			text(pixel2Second(i*graphWidth/72, graphWidth) + "Min" , i*graphWidth/72, _height-50);
			popStyle();
		} else {
			strokeWeight(1);
			line(i*graphWidth/72, _height-70, (i)*graphWidth/72, _height-90);
		} 	
	}


	for(int i=0; i<31; i++){
		if(i%5 == 0)  {
			
			strokeWeight(2);
			line(-10, graphHeight-i*graphHeight/30, 10, graphHeight-i*graphHeight/30);
			

			pushStyle();
			textAlign(RIGHT, CENTER);
			fill(#FFFFFF);
			noStroke();
			textFont(graphFont);
			text(pixel2Celcius(i*graphHeight/30, graphHeight) + "°C", -15, graphHeight-i*graphHeight/30);
			popStyle();
		}
	}
	
	popStyle();
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

int pixel2Second(float _pixelLength, float _pixelWidth){
	return (int)map(_pixelLength, 0, _pixelWidth, 0, 3600)/600;
}

int pixel2Celcius(float _pixelLength, float _pixelHeight){
	return (int)map(_pixelLength, 0, _pixelHeight, 0, MAX_TEMP);
}

float milliSec2Pixel(float _millis, float _pixelWidth){
	return map(_millis, 0, 360000, 0, _pixelWidth);
}

float celcius2Pixel(float _temp, float _pixelHeight){
	return map(_temp, 0, MAX_TEMP, 0, _pixelHeight);
}

void drawRealThermalGraph(float _width, float _height, float _margin){
	if(!bStart){ return;}
	else {
		float _xPos = map(millis()-startTimer, 0, 36000, 0, _width-_margin*2);
		float _yPos = _height - log(_xPos) * 50;
		// draw x,y line
		pushMatrix();
		translate(_margin, _margin);
		pushStyle();
		stroke(#00FF00);
		strokeWeight(4);

		point(_xPos, _yPos);
		

		popStyle();
		popMatrix();
	}
}

void serialEvent(Serial port){
	String inString = feather.readStringUntil('\n');


	println(inString);
	// if(inString != null){
	// 	inString = trim(inString);

	// 	if(inString.startsWith("S")){
	// 		startTimer = millis();
	// 	}
	// }

	// if(inString!=null){
	// 	inString = 
	// }
}

void keyReleased(){
	if(key == ' ' ) {
		if(!bStart){
			startTimer = millis();
			bStart = true;
		}
	}
}