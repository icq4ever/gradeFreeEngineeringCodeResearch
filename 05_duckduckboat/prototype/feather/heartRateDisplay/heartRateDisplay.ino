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


bool bUpdateSegmentReady = false;
byte getBuffer[2];

Adafruit_7segment matrix = Adafruit_7segment();

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
	matrix.begin(0x70);							// I2C address

	// join I2C
	// Wire.begin(8);				// join I2C as slave ID 8
	Wire.onReceive(receiveEvent);

	heartRate = 0;
}

void receiveEvent(int howMany){
	int LOWint = Wire.read();
	int HIGHint = Wire.read();
	
	heartRate = LOWint + HIGHint * 256;
	bUpdateSegmentReady = true;
	// Serial.println(heartRate);
}

void loop() {
	// try to print a number thats too long
	if(bUpdateSegmentReady){
		if(heartRate != 0){
			matrix.print(heartRate, DEC);
			matrix.writeDisplay();
		} else {
			matrix.writeDigitNum(0, 8);
			matrix.writeDigitNum(1, 8);
			matrix.writeDigitNum(3, 8);
			matrix.writeDigitNum(4, 8);
			matrix.writeDisplay();
		}
		bUpdateSegmentReady = false;
	}

	delay(100);

	// Serial.println(getMessage);
	

	// print a hex number
	// matrix.print(0xBEEF, HEX);
	// matrix.writeDisplay();
	// delay(500);

	// print a floating point 
	// matrix.print(12.34);
	// matrix.writeDisplay();
	// delay(500);

	// print with print/println
	// for (uint16_t counter = 0; counter < 9999; counter++) {
	// 	matrix.println(counter);
	// 	matrix.writeDisplay();
	// 	delay(10);
	// }

	// method #2 - draw each digit
	// uint16_t blinkcounter = 0;
	// boolean drawDots = false;
	// for (uint16_t counter = 0; counter < 9999; counter ++) {
	// 	matrix.writeDigitNum(0, (counter / 1000), drawDots);
	// 	matrix.writeDigitNum(1, (counter / 100) % 10, drawDots);
	// 	matrix.drawColon(drawDots);
	// 	matrix.writeDigitNum(3, (counter / 10) % 10, drawDots);
	// 	matrix.writeDigitNum(4, counter % 10, drawDots);

	// 	blinkcounter+=50;
	// 	if (blinkcounter < 500) {
	// 		drawDots = false;
	// 	} else if (blinkcounter < 1000) {
	// 		drawDots = true;
	// 	} else {
	// 		blinkcounter = 0;
	// 	}
	// 	matrix.writeDisplay();
	// 	delay(10);
	// }
}
