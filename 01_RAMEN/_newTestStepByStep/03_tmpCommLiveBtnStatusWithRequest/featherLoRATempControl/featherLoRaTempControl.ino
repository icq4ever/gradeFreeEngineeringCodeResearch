#include <RH_RF95.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"

// GFE STANDARD CONTROL PIN LIST
const int PIN_SOLENOID_1_BTN    = A2;       // 1, MSG
const int PIN_SOLENOID_2_BTN    = A3;       // 2, MSG
const int PIN_SOLENOID_3_BTN    = A4;       // 3, MSG
const int PIN_DROP_MBALL_BTN    = A5;       // 4, MSG   
const int PIN_HEATING_MBALL_BTN = 5;        // 8, MSG   
const int PIN_SERVO_UP_BTN      = 9;        // 9
const int PIN_SERVO_DOWN_BTN    = 6;        // 10


// I2C PORT EXPANDER
// 0  : PIN_START_BTN
// 1  : PIN_EGG_BREAKER_BTN
// 15 : PIN_HOT_WATER
Adafruit_MCP23017 mcp;


const int PIN_LED         = 13;

const int RFM95_CS      = 8;
const int RFM95_RST     = 4;
const int RFM95_INT     = 7;

const float RF95_FREQ   = 433.0;

const int NUM_OF_INPUT = 10;

int  inputPinList[NUM_OF_INPUT];

// == Thermalcoupler buffer, data setup ==========
const int RECV_BUFFER_MAX_SIZE = 20;
uint8_t recvBuffer[RECV_BUFFER_MAX_SIZE];

typedef union{
    float tempFloat;
    byte tempBin[4];    // union : float / 4 binary
} temperature;

temperature waterTemp, noodleTemp;
// ===============================================

// btn dummy array
bool inputBtnStatus[NUM_OF_INPUT];



RH_RF95 rf95(RFM95_CS, RFM95_INT);
unsigned long countTick;


void setup(){
    Serial.begin(115200);
    pinMode(PIN_LED, OUTPUT);
    mcp.begin();

    // init mcp input
    mcp.pinMode(0, INPUT);
    mcp.pullUp(0, HIGH);

    mcp.pinMode(1, INPUT);
    mcp.pullUp(1, HIGH);

    mcp.pinMode(15, INPUT);
    mcp.pullUp(15, HIGH);


    delay(1000);
    
    countTick = 0;

    // for(int i=0; i<10; i++){
    //     if(i%2 == 0)    inputBtnStatus[i] = true;
    //     else            inputBtnStatus[i] = false;
    // }


    // intput GPIO
    inputPinList[0] = PIN_SOLENOID_1_BTN;
    inputPinList[1] = PIN_SOLENOID_2_BTN;
    inputPinList[2] = PIN_SOLENOID_3_BTN;
    inputPinList[3] = PIN_DROP_MBALL_BTN;
    inputPinList[4] = PIN_HEATING_MBALL_BTN;
    inputPinList[5] = PIN_SERVO_UP_BTN;
    inputPinList[6] = PIN_SERVO_DOWN_BTN;

    // input Pin setup
    for(int i=0; i<NUM_OF_INPUT; i++){
        pinMode(inputPinList[i], INPUT_PULLUP);    
        inputBtnStatus[i] = false;
    }
    initLoRa();
}

void loop(){
    // delay(1000);
    countTick++;
    
    if(countTick > 20){
       requestTempToActionModule();
        // sendBtnStatus();
        countTick = 0;
        // delay(100);
    } 
    else {
        updateBtnStatusBuffer();
        sendBtnStatus();
       // delay(100);
    }

//    Serial.println(countTick);
}

void initLoRa() { // init
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);

    Serial.println("Feather LoRa RX Test!");

    // manual LoRa reset
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    while(!rf95.init()){
        Serial.println("LoRa radio init faild");
        while(1);
    }

    Serial.println("LoRa radio init OK!");

    // defauls after init are 434.0MHz. 
    if(!rf95.setFrequency(RF95_FREQ)){
        Serial.println("setFrequency failed.");
        while(1);
    }

    Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
    // you can set transmitter powers from 5 to 23 dBm:
    rf95.setTxPower(23, false);
}

void sendBtnStatus(){
//    Serial.println("btn sent!");
    char sendBtnPacket[sizeof(inputBtnStatus) + 3];

    sendBtnPacket[0] = '/';
    sendBtnPacket[1] = 'B';
    for(int i=0; i<sizeof(inputBtnStatus); i++){
        if(inputBtnStatus[i])    sendBtnPacket[i+2] = '1';
        else                sendBtnPacket[i+2] = '0';
    }
    sendBtnPacket[sizeof(sendBtnPacket) -1] = 0;

    rf95.send((uint8_t *) sendBtnPacket, sizeof(sendBtnPacket));
//    rf95.waitPacketSent();
}

void requestTempToActionModule(){
    char requestPacket[3] = "/R";
    requestPacket[2] = 0;

    digitalWrite(PIN_LED, HIGH);
    rf95.send((uint8_t *) requestPacket, 3);

    rf95.waitPacketSent();
    digitalWrite(PIN_LED, LOW);
    receiveTempFromActionModule();
        
    Serial.println("req sent!");
}

void receiveTempFromActionModule(){
    if(rf95.waitAvailableTimeout(500)){
        uint8_t recvBufferLen = sizeof(recvBuffer);

        if(rf95.recv((char *)recvBuffer, &recvBufferLen))   {   Serial.println("Received OK!");     }
        else                                                {   Serial.println("Receive Failed");   }

        if(recvBuffer[0] == '/'){
            waterTemp.tempBin[0] = recvBuffer[1];
            waterTemp.tempBin[1] = recvBuffer[2];
            waterTemp.tempBin[2] = recvBuffer[3];
            waterTemp.tempBin[3] = recvBuffer[4];

            noodleTemp.tempBin[0] = recvBuffer[5];
            noodleTemp.tempBin[1] = recvBuffer[6];
            noodleTemp.tempBin[2] = recvBuffer[7];
            noodleTemp.tempBin[3] = recvBuffer[8];
        }

        Serial.print(waterTemp.tempFloat, 4);
        Serial.print(", ");
        Serial.print(noodleTemp.tempFloat, 4);
        Serial.println();
    } 
    else {
        Serial.println("Action Module Not reply.........");
    }
}

void printTempData(){
    Serial.print(waterTemp.tempFloat, 4);
    Serial.print(", ");
    Serial.print(noodleTemp.tempFloat, 4);
    Serial.println();
}

void updateBtnStatusBuffer(){
    for(int i=0; i<7; i++){
        if(digitalRead(inputPinList[i]) == LOW)   inputBtnStatus[i] = true;
        else                                      inputBtnStatus[i] = false;
    }

    // 0  : PIN_START_BTN
    // 1  : PIN_EGG_BREAKER_BTN
    // 15 : PIN_HOT_WATER
    if(mcp.digitalRead(0))  inputBtnStatus[7] = false;
    else                    inputBtnStatus[7] = true;

    if(mcp.digitalRead(1))  inputBtnStatus[8] = false;
    else                    inputBtnStatus[8] = true;

    if(mcp.digitalRead(15)) inputBtnStatus[9] = false;
    else                    inputBtnStatus[9] = true;
    
}
