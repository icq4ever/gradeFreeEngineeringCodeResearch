import processing.serial.*;

// GForceVis gVis;
GForceVis deltaVis;
TwoDimensionGraph XVis;
TwoDimensionGraph YVis;
TwoDimensionGraph ZVis;
float GFMin, GFMax;

float deltaX, deltaY, deltaZ;
float lastDeltaX, lastDeltaY, lastdeltaZ;
float delta;

PFont interfaceFont;

// boolean buttonState = false;

boolean cont;
boolean bGraphOn;

TimeTemplate clock;
ShootHistoryBook shootLog;

Serial port;

boolean bStablized = false;
float lastStablizedTimer;
float size;
// todo : export to json for check lately

int shootBtnStatus = 0;
int lastShootBtnStatus = 0;
int numberOfBuffer = 500;

static float stablizeThreshold = 0.15;


void setup() {
	size(1600,900, P3D);
	frameRate(60);
	clock = new TimeTemplate();
	shootLog = new ShootHistoryBook();

	interfaceFont = loadFont("ShareTechMono-Regular-24.vlw");

	GFMin = 256;
	GFMax = 640;
	// gVis = new GForceVis(300, 300, -10, 10, 4);
	deltaVis = new GForceVis(600, 600, -10, 10, 8);
	XVis = new TwoDimensionGraph("X-AXIS", 300, 200, -10, 10);
	YVis = new TwoDimensionGraph("Y-AXIS", 300, 200, -10, 10);
	ZVis = new TwoDimensionGraph("Z-AXIS", 300, 200, -10, 10);

	cont = false;
	bGraphOn = false;

	// port = new Serial(this, Serial.list()[0], 115200);
	port = new Serial(this, "COM6", 115200);
	// port.bufferUntil('\n');
	
	textFont(interfaceFont, 24);
}

void draw() {
	background(10, 10, 10);
	updateBG();

	// if (cont)    {
	pushData(deltaX, deltaY, deltaZ);
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
			
			if(i<9)		text("0"+ Integer.toString(i+1), 40, 200+i*24);
			else  		text(i+1, 40, 200+i*24);
			
			
			if(abs(shootLog.history.get(i).x) < stablizeThreshold)	fill(0, 255, 0);
			else  													fill(255, 0, 0);
			// text(shootLog.history.get(i).x, 240, 400+i*18);
			text(shootLog.history.get(i).x, 100, 200+i*24);

			if(abs(shootLog.history.get(i).y) < stablizeThreshold)	fill(0, 255, 0);
			else  													fill(255, 0, 0);
			// text(shootLog.history.get(i).y, 320, 400+i*18);
			text(shootLog.history.get(i).y, 200, 200+i*24);

			if(abs(shootLog.history.get(i).z) < stablizeThreshold)	fill(0, 255, 0);
			else  													fill(255, 0, 0);
			// text(shootLog.history.get(i).z, 400, 400+i*18);
			text(shootLog.history.get(i).z, 300, 200+i*24);

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
		// port.write('1');
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
	try{
		String inString = port.readStringUntil('\n');
		
		if(inString != null){
			inString = trim(inString);
			String[] values = inString.split(","); 

			deltaX = Float.parseFloat(values[0]);
			deltaY = Float.parseFloat(values[1]);
			deltaZ = Float.parseFloat(values[2]);

			lastShootBtnStatus = shootBtnStatus;

			if(values[3].equals("1"))	shootBtnStatus = 1;
			else  						shootBtnStatus = 0;
			
			if(shootBtnStatus != lastShootBtnStatus && shootBtnStatus == 1 )	{
				cont = true;
			} 

			delta = (abs(deltaX) + abs(deltaY) + abs(deltaZ)) / 3;
		}
	} catch(RuntimeException e) {
		// e.printStackTrace();
	}
}

void updateBG(){
	if(delta>stablizeThreshold){
		bStablized = false;
		size = map(abs(delta), stablizeThreshold, 5, 50, 600);
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
	
	pushStyle();
	fill(255);
	textAlign(LEFT, TOP);
	text(delta, 40, 40);

	fill(255);
	textAlign(RIGHT, TOP);
	text(int(frameRate) + " fps", width-40, 40);
	popStyle();
}

void exit(){
	port.stop();
	super.exit();
}