/*
	gfe castaway GPX BOX

	- SSD1325 OLED 2.7inch display hardware
		: HW SPI MODE

	- Adafuit feather ultimate GPS module
	
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1325.h>
#include <Adafruit_GPS.h>

// GPS info demo trigger
#define DEMO 0

#define FONT_W	6
#define FONT_H	8

// we'll using hardware SPI.
#define OLED_CS A3
#define OLED_RESET A4
#define OLED_DC A2

#define GPSSerial Serial1

// this is for hardware SPI, fast! but fixed oubs
Adafruit_SSD1325 display(OLED_DC, OLED_RESET, OLED_CS);

Adafruit_GPS GPS(&GPSSerial);
#define GPSECHO false

uint32_t timer = millis();

/* settings for our little animation later */
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

// display information 
String latitude;
String longitude;
String altitude;
String hdop_info;
int fixQ;

static const unsigned char PROGMEM logo16_glcd_bmp[] = 
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
  
// The Arduino UNO doesnt have enough RAM for gradients
// but the *display* supports it!
void graydient()
{
	unsigned char x,y;
	display.command(0x15); /* set column address */
	display.command(0x00); /* set column start address */
	display.command(0x3f); /* set column end address */
	display.command(0x75); /* set row address */
	display.command(0x00); /* set row start address */
	display.command(0x3f); /* set row end address */
	for(y=0;y<64;y++) {
		for(x=0;x<4;x++) {
			display.data(0x00);
			display.data(0x11);
			display.data(0x22);
			display.data(0x33);
			display.data(0x44);
			display.data(0x55);
			display.data(0x66);
			display.data(0x77);
			display.data(0x88);
			display.data(0x99);
			display.data(0xAA);
			display.data(0xBB);
			display.data(0xCC);
			display.data(0xDD);
			display.data(0xEE);
			display.data(0xFF);
		}
	}
}

void setup()   {                
	Serial.begin(115200);
	Serial.println("GFE GPS module test!");

	GPS.begin(9600);

	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	display.begin();
	// init done

	// display.display(); // show splashscreen
	display.clearDisplay();   // clears the screen and buffer

	display.setRotation(0);



	GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); 
	GPS.sendCommand(PGCMD_ANTENNA);

	delay(1000);
	GPSSerial.println(PMTK_Q_RELEASE);
}


void loop() {
	char c = GPS.read();

	if(GPSECHO)
		if(c) Serial.print(c);

	if (GPS.newNMEAreceived()) {
	   // Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    	if (!GPS.parse(GPS.lastNMEA())){ // this also sets the newNMEAreceived() flag to false
			return; // we can fail to parse a sentence in which case we should just wait for another
		}
 	}

	if(timer>millis())	timer = millis();

	if(millis() - timer > 2000){
		timer = millis();

		// debug
		Serial.print("\nTime: ");
	    Serial.print(GPS.hour, DEC); Serial.print(':');
	    Serial.print(GPS.minute, DEC); Serial.print(':');
	    Serial.print(GPS.seconds, DEC); Serial.print('.');
	    Serial.println(GPS.milliseconds);
	    Serial.print("Date: ");
	    Serial.print(GPS.day, DEC); Serial.print('/');
	    Serial.print(GPS.month, DEC); Serial.print("/20");
	    Serial.println(GPS.year, DEC);
	    Serial.print("Fix: "); Serial.print((int)GPS.fix);
	    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
	    fixQ = (int)GPS.fixquality;
	    if (GPS.fix) {

	    	// debug
			Serial.print("Location: ");
			Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
			Serial.print(", ");
			Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
			Serial.print("Speed (knots): "); Serial.println(GPS.speed);
			Serial.print("Angle: "); Serial.println(GPS.angle);
			Serial.print("Altitude: "); Serial.println(GPS.altitude);
			Serial.print("Satellites: "); Serial.println((int)GPS.satellites);

			latitude = " " + String(GPS.latitude/100, 6) + " " + GPS.lat;
			longitude = String(GPS.longitude/100, 6) + " " + GPS.lon;
			hdop_info = "HDOP : " + String(GPS.HDOP, 2);


			if((int)GPS.fixquality > 1)	altitude = String(GPS.altitude) + " M" ;
			else 						altitude = "-";

		} else {
			latitude = "-";
			longitude = "-";
			altitude = "-";
			hdop_info = "HDOP : - ";
		}


		if(DEMO != 0){
			latitude = " 37.334126 N";
			longitude = "126.541500 E";
			altitude = "33.60 M";
			hdop_info = "HDOP : 0.90";
			fixQ = 2;
		}

		drawGPSInformation();
	}

	
	
}


// for splash
void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
	uint8_t icons[NUMFLAKES][3];

	randomSeed(666);     // whatever seed


	// initialize
	for (uint8_t f=0; f< NUMFLAKES; f++) {
		icons[f][XPOS] = random(display.width());
		icons[f][YPOS] = 0;
		icons[f][DELTAY] = random(5) + 1;

		Serial.print("x: ");
		Serial.print(icons[f][XPOS], DEC);
		Serial.print(" y: ");
		Serial.print(icons[f][YPOS], DEC);
		Serial.print(" dy: ");
		Serial.println(icons[f][DELTAY], DEC);
	}

	while (1) {
		// draw each icon
		for (uint8_t f=0; f< NUMFLAKES; f++) {
			display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE);
		}
		display.display();
		delay(200);
		
		// then erase it + move it
		for (uint8_t f=0; f< NUMFLAKES; f++) {
			display.drawBitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, BLACK);
			// move it
			icons[f][YPOS] += icons[f][DELTAY];
			// if its gone, reinit
			if (icons[f][YPOS] > display.height()) {
				icons[f][XPOS] = random(display.width());
				icons[f][YPOS] = 0;
				icons[f][DELTAY] = random(5) + 1;
			}
		}
	}
}

void drawGPSInformation(){
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextWrap(false);
	display.setTextColor(WHITE);

	// display resolution : 128 * 64
	switch(fixQ){
		case 0:
			// display.clearDisplay();
			display.drawPixel(0, display.height()-1, WHITE);
			// display.display();
			break;
		case 1:
			// display.clearDisplay();
			display.drawPixel(0, display.height()-1, WHITE);
			display.drawPixel(2, display.height()-1, WHITE);
			// display.display();
			break;
		case 2:
			// display.clearDisplay();
			display.drawPixel(0, display.height()-1, WHITE);
			display.drawPixel(2, display.height()-1, WHITE);
			display.drawPixel(4, display.height()-1, WHITE);
			// display.display();
			break;
		default:
			break;
	}

	display.setCursor(FONT_W *10, FONT_H * 7);
	for(int i=0; i<hdop_info.length(); i++){
		display.write(hdop_info.charAt(i));
	}
	

	display.setCursor(FONT_W*3, FONT_H * 1);
	for(int i=0; i<latitude.length(); i++){
		display.write(latitude.charAt(i));
	}

	display.setCursor(FONT_W*3, FONT_H * 3);
	for(int i=0; i<latitude.length(); i++){
		display.write(longitude.charAt(i));
	}

	display.setCursor(FONT_W*3, FONT_H * 5);
	for(int i=0; i<latitude.length(); i++){
		display.write(altitude.charAt(i));
	}	
	display.display();
}

// tested
void drawString(String message){
	display.clearDisplay();
	String elapsedMillis = message + String(millis()) ;

	display.setTextSize(1);
	display.setTextWrap(false);
	display.setTextColor(WHITE);
	display.setCursor(0,0);

	for(int i=0; i<elapsedMillis.length(); i++){
		display.write(elapsedMillis.charAt(i));
		if((i>0) && (i%21 == 0)){
			display.println();
		}
	}
	display.display();
}

void testdrawchar(void) {
	display.setTextSize(1);
	display.setTextWrap(false);
	display.setTextColor(WHITE);
	display.setCursor(0,0);

	for (int16_t i=0; i < 168; i++) {
		if (i == '\n') continue;
	display.write(i);
	if ((i > 0) && (i % 21 == 0))
		display.println();
	}    
	display.display();
}