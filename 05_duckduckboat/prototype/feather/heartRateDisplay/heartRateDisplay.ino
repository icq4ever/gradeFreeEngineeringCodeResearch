/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED 7-Segment backpacks 
  ----> http://www.adafruit.com/products/881
  ----> http://www.adafruit.com/products/880
  ----> http://www.adafruit.com/products/879
  ----> http://www.adafruit.com/products/878

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"


bool bUpdateSegmentReady = false;						// flag var for prevent master/slave conflicts
byte getBuffer[2];

// join as slave ID  8
Adafruit_7segment matrix = Adafruit_7segment();		// modified LEDbackpack library. check "Adafruit_LEDBackpack.cpp line:204"

int heartRate;

void setup() {
#ifndef __AVR_ATtiny85__
	Serial.begin(115200);
	Serial.println("7 Segment Backpack Test");
	Serial.println("=======================");
#endif
	Serial.begin(115200);
	Serial.println("7 Segment Backpack Test");
	Serial.println("=======================");
	matrix.begin(8, 0x70);							// set slave ID / I2C address

	// join I2C
	// Wire.begin(8);				// this is moved to Adafruit_7segment constructor
	Wire.onReceive(receiveEvent);

	heartRate = 0;
}

void receiveEvent(int howMany){
	int LOWint = Wire.read();
	int HIGHint = Wire.read();
	
	heartRate = LOWint + HIGHint * 256;
	bUpdateSegmentReady = true;						// flag on!						
	// Serial.println(heartRate);
}

void loop() {
	// try to print a number thats too long
	if(bUpdateSegmentReady){						// GUI update when flag on!!
		if(heartRate != 0){
			matrix.print(heartRate, DEC);
			matrix.writeDisplay();
		} else {
			matrix.writeDigitNum(0, 0);
			matrix.writeDigitNum(1, 0);
			matrix.writeDigitNum(3, 0);
			matrix.writeDigitNum(4, 0);

			// matrix.print('-', 0);
			// matrix.print('-', 1);
			// matrix.print('-', 2);
			// matrix.print('-', 3);
			matrix.writeDisplay();
		}
		bUpdateSegmentReady = false;				// flag off
	}

	delay(100);
}
