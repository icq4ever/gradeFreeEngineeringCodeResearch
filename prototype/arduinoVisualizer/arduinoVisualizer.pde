import processing.serial.*;

// GForceVis gVis;
GForceVis deltaVis;
TwoDimensionGraph XVis;
TwoDimensionGraph YVis;
float GFMin, GFMax;

float sensorX, sensorY, sensorZ;
float lastSensorX, lastSensorY, lastSensorZ;
float deltaX, deltaY, deltaZ;
float delta;

boolean buttonState = false;

boolean cont;
boolean bGraphOn;

TimeTemplate clock;
ShootHistoryBook shootLog;

Serial port;

float blinkTimer;
float lastBlinkCheckTimer;
boolean bBlinkOn;
boolean bReviewOn;
boolean bStablized = false;
float lastStablizedTimer;
float size;
// todo : export to json for check lately

int numberOfBuffer = 500;


void setup() {
	size(1920, 1080);
	frameRate(60);
	clock = new TimeTemplate();
	shootLog = new ShootHistoryBook();

	sensorX = 0;
	sensorY = 0;
	sensorZ = 0;
	lastSensorX = 0;
	lastSensorY = 0;
	lastSensorZ = 0;

	GFMin = 256;
	GFMax = 640;
	// gVis = new GForceVis(300, 300, -10, 10, 4);
	deltaVis = new GForceVis(600, 600, -20, 20, 5);
	XVis = new TwoDimensionGraph("X-AXIS", 550, 300, -10, 10);
	YVis = new TwoDimensionGraph("Y-AXIS", 550, 300, -10, 10);

	cont = false;
	bGraphOn = false;

	// port = new Serial(this, Serial.list()[0], 115200);
	port = new Serial(this, "COM6", 115200);
	port.bufferUntil('\n');

	lastBlinkCheckTimer = millis();
	bBlinkOn = true;
	
}

void draw() {
	background(10, 10, 10);
	updateBG();

	// if (cont)    {
	pushData(sensorX, sensorY);
	pushDeltaData(deltaX, deltaY);
	// }
	
	review();

	
	// gVis.draw(250, 200);
	deltaVis.draw(width/2, height/2);

	if(bGraphOn) {
		XVis.draw(width/4, 200);
		YVis.draw(width/4, 600);
	}

	clock.update();
	pushStyle();
	clock.draw(40, height-60, false);
	popStyle();

	if (shootLog.getSizeOfRecord() !=0) {
		for (int i=0; i<shootLog.getSizeOfRecord(); i++) {
			pushStyle();
			fill(255, 255, 0);
			text(shootLog.timeStamp.get(i), 40, 400+i*18);
			fill(0, 255, 0);
			text(shootLog.history.get(i).x, 200, 400+i*18);
			text(shootLog.history.get(i).y, 280, 400+i*18);
			popStyle();
		}
	}
}

void pushData(float _dataX, float _dataY) {
	PVector data = new PVector(_dataX, _dataY);
	// gVis.pushData(data);
	XVis.pushData(data.x);
	YVis.pushData(data.y);
}


void pushDeltaData(float _deltaX, float _deltaY){
	PVector data = new PVector(_deltaX, _deltaY);
	deltaVis.pushData(data);
}

void review(){
	if (cont) {
		PVector _t = new PVector(sensorX, sensorY);
		String _now = clock.getDateTime();

		shootLog.pushRecord(_t, _now);
		// gVis.record(_t);

		cont = false;
	}
}

void keyPressed() {
	if (key == 'c' || key == 'C') {
		cont = true;
	}

	if(key == 'g' || key == 'G'){
		bGraphOn = !bGraphOn;
	}

	if(key == ' ' ){
		bReviewOn = !bReviewOn;
	}
}

// Serial Event
void serialEvent(Serial port){
	String inString = port.readStringUntil('\n');
	
	if(inString != null){
		inString = trim(inString);
		String[] values = inString.split(","); 

		lastSensorX = sensorX;
		lastSensorY = sensorY;
		lastSensorZ = sensorZ;

		sensorX = Float.parseFloat(values[0]);
		sensorY = Float.parseFloat(values[1]);
		sensorZ = Float.parseFloat(values[2]);

		deltaX = sensorX - lastSensorX;
		deltaY = sensorY - lastSensorY;
		deltaZ = sensorZ - lastSensorZ;

		delta = (abs(deltaX) + abs(deltaY) + abs(deltaZ)) / 3;
	}
}

void updateBG(){
	// blinkTimer  = map(delta, 0, 5, 300, 1000);

	if(delta>0.4){
		bStablized = false;
		size = map(delta, 1.2, 10, 0, 600);
		fill(#FF0000);
		noStroke();
		ellipse(width/2, height/2, size, size);
		size = 0;
	} else {
		if(!bStablized)		{
			lastStablizedTimer = millis();
			bStablized = true;
		}
		float targetSize = constrain(map(millis() - lastStablizedTimer, 0, 100, 0, 600), 0, 600);
		
		float alpha = map(millis() - lastStablizedTimer, 0, 100, 50, 80);
		size += (targetSize - size)*0.09;

		if(size <590 )	fill(#47C747, alpha);
		else  			fill(#47C747);
		noStroke();
		ellipse(width/2, height/2, size, size);
	}
	// if(delta>1.2 && delta < 3.6){
	// 	if(millis() - lastBlinkCheckTimer < 200)	{
	// 		lastBlinkCheckTimer = millis();
	// 		bBlinkOn = !bBlinkOn;
		
	// 	}
	// 	if(bBlinkOn)	fill(255, 0, 0);
	// 	else 			fill(0);
	// 	noStroke();
	// 	rect(0, 0, width, height);
	// } else if (delta >= 3.6 ) {
	// 	if(millis() - lastBlinkCheckTimer < 50)	{
	// 		lastBlinkCheckTimer = millis();
	// 		bBlinkOn = !bBlinkOn;
		
	// 	}
	// 	if(bBlinkOn)	fill(255, 0, 0);
	// 	else 			fill(0);
	// 	noStroke();
	// 	rect(0, 0, width, height);

	// 	// fill(0);
	// 	// text(delta;)
	// } else {
	// 	fill(#47C747);
	// 	noStroke();
	// 	rect(0, 0, width, height);
	// }
	fill(0);
	text(delta, 20, 20);
}

void exit(){
	port.stop();
	super.exit();
}