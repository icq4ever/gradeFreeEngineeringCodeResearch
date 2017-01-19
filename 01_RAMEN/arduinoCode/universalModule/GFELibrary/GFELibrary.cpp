#inlcude "Arduino.h"
#include "GFELibrary.h"

GFELibrary::GFELibrary(int _s0, int _s1, int _s2, int _s3, int _readPin){
	s0 = _s0;
	s1 = _s1;
	s2 = _s2;
	s3 = _s3;
	readPin = _readPin;

	pinMode(s0, OUTPUT);
	pinMode(s1, OUTPUT);
	pinMode(s2, OUTPUT);
	pinMode(s3, OUTPUT);

	digitaWrite(s0, LOW);
	digitaWrite(s1, LOW);
	digitaWrite(s2, LOW);
	digitaWrite(s3, LOW);
}

GFELibrary::loop(){
		
}

void GFELibrary::readMux(int gfePinNo){
	int controlPin[] = {s0, s1, s2, s3};
	int muxChannel[16][4]={
		{0,0,0,0}, //channel 0
	    {1,0,0,0}, //channel 1
	    {0,1,0,0}, //channel 2
	    {1,1,0,0}, //channel 3
	    {0,0,1,0}, //channel 4
	    {1,0,1,0}, //channel 5
	    {0,1,1,0}, //channel 6
	    {1,1,1,0}, //channel 7
	    {0,0,0,1}, //channel 8
	    {1,0,0,1}, //channel 9
	    {0,1,0,1}, //channel 10
	    {1,1,0,1}, //channel 11
	    {0,0,1,1}, //channel 12
	    {1,0,1,1}, //channel 13
	    {0,1,1,1}, //channel 14
	    {1,1,1,1}  //channel 15
	};

	for(int i = 0; i < 4; i ++){
    	digitalWrite(controlPin[i], muxChannel[channel][i]);
  	}

  	return analogRead(readPin);
  }