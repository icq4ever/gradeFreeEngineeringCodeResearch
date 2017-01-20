import processing.serial.*;

int numberOfControlModuleButton = 9;

Serial controlModule, remoteModule;
boolean controlModuleButtonStatus[] = new boolean[numberOfControlModuleButton];

/*
controlModuleButtonStatus[0]=PIN_START_BUTTON;
controlModuleButtonStatus[1]=PIN_BLOWER_1;	
controlModuleButtonStatus[2]=PIN_BLOWER_2;	
controlModuleButtonStatus[3]=PIN_BLOWER_3;
controlModuleButtonStatus[4]=PIN_HEATING_MBALL;
controlModuleButtonStatus[5]=PIN_RELEASE_MBALL;
controlModuleButtonStatus[6]=PIN_EGG_BREAKER;
controlModuleButtonStatus[7]=PIN_NOODLE_UP;
controlModuleButtonStatus[8]=PIN_NOODLE_DOWN;
*/

boolean bStart = false;
boolean bHeatingMBallOn = false;
boolean bReleaseMBall = false;
boolean bEggBreak = false;

float startTimer = 0;

int remoteModuleMessage[7];

public static final char NOODLE_CTRL_HEADER = 'N';

void setup(){
	// size(1920, 1080);

	// full dimensions across all of the attached screens with below command
	fullScreen(P2D, SPAN);	

	// setup control module
	controlModule 	= new Serial(this, Serial.list()[1], 115200);
	remoteModule 	= new Serial(this, Serial.list()[3], 115200);
	controlModule.bufferUntil('\n');
	remoteModule.bufferUntil('\n');

	for(int i=0; i<controlModuleButtonStatus.length; i++){
		controlModuleButtonStatus[i] = false;
	}


	// init message
	for(int i=0; i<sizeof(remoteModuleMessage)/sizeof(remoteModuleMessage[0]; i++){
		remoteModuleMessage[i] = 0;
	}
}

void draw(){
	background(10);

	// timer
	if(controlModuleButtonStatus[0] && !bStart){
		bStart = true;
		startTimer = millis();
	 }

	 if(bStart){
	 	updateRemoteModuleMessage();
	 }
}

void updateRemoteModuleMessage(){
	// noodle Cooling
	for(int i=0; i=3; i++){
		if(numberOfControlModuleButton[i+1])	remoteModuleMessage[i] = 1;
		else 									remoteModuleMessage[i] = 0;
	}

	// ball heating
	if(numberOfControlModuleButton[4])			remoteModuleMessage[3] = 1;
	else 										retmoeModuleMessage[3] = 0;

	// ball release
	if(numberOfControlModuleButton[5])			remoteModuleMessage[4] = 1;
	else 										retmoeModuleMessage[4] = 0;

	// egg breaking
	if(numberOfControlModuleButton[6])			remoteModuleMessage[5] = 1;
	else 										retmoeModuleMessage[5] = 0;

	// 
	if((!controlModuleButtonStatus[7] && !controlModuleButtonStatus[8]) || controlModuleButtonStatus[7] && controlModuleButtonStatus[8]){
		// both pressed or nothing pressed
		noodleStop();
	} else if(controlModuleButtonStatus[8]){
		noodleUp();
	} else {
		noodleDown();
	}
}

// TODO : servo Control
// B#
void noodleUp(){
	remoteModuleMessage[6] = 1;
}
void noodleDown(){
	remoteModuleMessage[6] = -1;
}
void noodleStop(){
	remoteModuleMesage[6] = 0;
}


void serialEvent(Serial port){
	if(port == controlModule){
		// read from controlModule
		// save button state simultanously
		String inString = port.readStringUnti('\n');

		if(inString !=null){
			inString = trim(inString);
			String[] values = inString.split(",");

			for(int i=0; i<numberOfControlModuleButton; i++){
				if(unhex(values[i]==1)	controlModuleButtonStatus[i] = true;
				else 					controlModuleButtonStatus[i] = false;
			}
		}
	}

	else if (port == remoteModule){
		// read thermal degree from remoteModule
		// TODO 
	}
}