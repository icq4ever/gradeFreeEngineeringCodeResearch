     // Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

#include <SPI.h>
#include <RH_RF95.h>

/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7



// Change to 433.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int status_led = 13;
char radiopacket[20] ={0};
int getMessageFromP5;

int bCupIsEmpty = 1;
int handInHandRate = 0;
int bHandOn = 0;


void setup() {
  pinMode(13, OUTPUT);
  pinMode(A5, INPUT);        // drinking AMBASA

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  pinMode(status_led, OUTPUT);
  digitalWrite(status_led, LOW);


  //while (!Serial);
  Serial.begin(9600);
  delay(100);

  // Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    // Serial.println("LoRa radio init failed");
    while (1);
  }
  // Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    // Serial.println("setFrequency failed");
    while (1);
  }
  // Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop()
{
  if(digitalRead(A5)) bCupIsEmpty = 0;
  else                bCupIsEmpty = 1;

  handInHandRate = analogRead(A1);
  if(handInHandRate > 200)  bHandOn = 1;
  else                      bHandOn = 0;


  // read message from processing via P5
  if(Serial.available()){
    getMessageFromP5 = Serial.read();
    if(getMessageFromP5 == 'f'){
      sendMsg('F');
      // Serial.println("F!");
      // digitalWrite(13, HIGH);
      // delay(2000);
    }

    if(getMessageFromP5 == 'b'){
      sendMsg('B');
      // Serial.println("B!");
      // digitalWrite(13, LOW);
      // delay(2000);
    }

    if(getMessageFromP5 == 'l'){ 
      sendMsg('L');
      // Serial.println("L!");
      // digitalWrite(13, HIGH);
      // delay(2000);
    }

    if(getMessageFromP5 == 'r'){ 
      sendMsg('R');
      // Serial.println("R!");
      // digitalWrite(13, LOW);
      // delay(2000);
    }
  }

  

  sendToP5();
}

void sendMsg(char m)
{
  radiopacket[0] ='/';
  radiopacket[1] = m;
  
  //itoa(packetnum++, radiopacket+13, 10);
  //Serial.print("Sending "); Serial.println(radiopacket);
  
  
  //Serial.println("Sending..."); 
  delay(10);
  rf95.send((uint8_t *)radiopacket, 2);

  // Serial.println("Waiting for packet to complete..."); 
  delay(10);
  
}

// send to P5
void sendToP5(){
  Serial.print(bCupIsEmpty);
  Serial.print(",");
  Serial.print(bHandOn);
  Serial.println();
}
