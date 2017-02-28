import processing.serial.*;

// GForceVis gVis;
GForceVis deltaVis;
TwoDimensionGraph XVis;
TwoDimensionGraph YVis;
TwoDimensionGraph ZVis;
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

static float stablizeThreshold = 0.4;


void setup() {
	size(1600,900);
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
	XVis = new TwoDimensionGraph("X-AXIS", 300, 200, -10, 10);
	YVis = new TwoDimensionGraph("Y-AXIS", 300, 200, -10, 10);
	ZVis = new TwoDimensionGraph("Z-AXIS", 300, 200, -10, 10);

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
	pushData(sensorX, sensorY, sensorZ);
	pushDeltaData(deltaX, deltaY, deltaZ);
	// }
	
	review();

	
	// gVis.draw(250, 200);
	

	if(bGraphOn) {
		XVis.draw(width-40-300, 200);
		YVis.draw(width-40-300, 420);
		ZVis.draw(width-40-300, 640);
	}

	clock.update();

	pushStyle();
	clock.draw(40, height-60, false);
	popStyle();

	printShootLog();

	deltaVis.draw(width/2, height/2);
}

void pushData(float _dataX, float _dataY, float _dataZ) {
	PVector data = new PVector(_dataX, _dataY, _dataZ);
	// gVis.pushData(data);
	XVis.pushData(data.x);
	YVis.pushData(data.y);
	ZVis.pushData(data.z);
}


void printShootLog(){
	if (shootLog.getSizeOfRecord() !=0) {
		for (int i=0; i<shootLog.getSizeOfRecord(); i++) {
			pushStyle();
			fill(255, 255, 0);
			// text(shootLog.timeStamp.get(i), 40, 400+i*18);
			
			if(i<9)		text("0"+ Integer.toString(i+1), 40, 200+i*18);
			else  		text(i+1, 40, 200+i*18);
			
			
			if(abs(shootLog.history.get(i).x) < stablizeThreshold)	fill(0, 255, 0);
			else  													fill(255, 0, 0);
			// text(shootLog.history.get(i).x, 240, 400+i*18);
			text(shootLog.history.get(i).x, 100, 200+i*18);

			if(abs(shootLog.history.get(i).y) < stablizeThreshold)	fill(0, 255, 0);
			else  													fill(255, 0, 0);
			// text(shootLog.history.get(i).y, 320, 400+i*18);
			text(shootLog.history.get(i).y, 180, 200+i*18);

			if(abs(shootLog.history.get(i).z) < stablizeThreshold)	fill(0, 255, 0);
			else  													fill(255, 0, 0);
			// text(shootLog.history.get(i).z, 400, 400+i*18);
			text(shootLog.history.get(i).z, 260, 200+i*18);

			popStyle();


			// record drawing on target
			pushMatrix();
			translate(width/2, height/2);
			pushStyle();
			noFill();
			stroke(#FF00FF);
			strokeWeight(10);
			point(map(shootLog.history.get(i).x, -10, 10, -300, 300), map(shootLog.history.get(i).y, -10, 10, -300, 300));
			popStyle();
			popMatrix();
		}
	}
}

void pushDeltaData(float _deltaX, float _deltaY, float _deltaZ){
	PVector data = new PVector(_deltaX, _deltaY, _deltaZ);
	deltaVis.pushData(data);
}

void review(){
	if (cont) {
		PVector _t = new PVector(deltaX, deltaY, deltaZ);
		String _now = clock.getDateTime();

		shootLog.pushRecord(_t, _now);
		// gVis.record(_t);

		cont = false;
	}
}

void keyPressed() {
	if (key == 'c' || key == 'C') {
		cont = true;
		port.write('1');
	}

	if(key == 'g' || key == 'G'){
		bGraphOn = !bGraphOn;
	}

	if(key == ' '){
		shootLog.clear();
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

	if(delta>stablizeThreshold){
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
	
	fill(0);
	text(delta, 20, 20);
}

void exit(){
	port.stop();
	super.exit();
}