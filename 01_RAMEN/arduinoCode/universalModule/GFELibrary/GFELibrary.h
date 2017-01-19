#ifndef GFELibrary_h
#define GFELibrary_h

#define MAX_PIN 	16

#include "Arduino.h"

class GFELibrary{
public:
	GFELibrary();
	setNumberOfOutput(int _numberOfoutputs);
	int readMux(int gfePinNo);

private:
	int s0, s1, s2, s3, readPin;
}