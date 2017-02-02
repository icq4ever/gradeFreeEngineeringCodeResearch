#include <SPI.h>
#include <RH_RF95.h>
#include <TimerOne.h>

const int PIN_LED = 13;

// LoRa Settings
const int RFM95_CS          = 8;
const int RFM95_RST         = 4;
const int RFM95_INT         = 7;

// LoRa FREQ SETTING
const float RF95_FREQ           = 433.0;

// number of button message bit
const int NUM_OF_INPUT      = 10;
    
const int IN_MESSAGE_SIZE   = 8;
const int RECV_BUFFER_SIZE  = IN_MESSAGE_SIZE+2;

const int OUT_MESSAGE_SIZE  = 12;
const int SEND_BUFFER_SIZE  = OUT_MESSAGE_SIZE+3;

// singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// timer flags
bool    bSendToAction, bRequestToAction, bSendToP5;

int     inputPinList[NUM_OF_INPUT];
int     inputBtnStatus[OUT_MESSAGE_SIZE];

char    sendBuffer[SEND_BUFFER_SIZE];
uint8_t recvBuffer[RECV_BUFFER_SIZE];

typedef union{
    float floatPoint;
    byte bin[4];
} temp;

temp waterTemp, noodleTemp;
// volatile unsigned long tickCount = 0;
volatile unsigned long  tcSendToAction, tcRequestToAction, tcSendToP5;

void tickCountUp(){
	tcSendToAction++;
	tcRequestToAction++;
	tcSendToP5++;
}

void setup(){
	pinMode(RFM95_RST, OUTPUT);
	digitalWrite(RFM95_RST, HIGH);

	delay(1000);

	// init timer flags
    bSendToAction = bRequestToAction = bSendToP5 = false;

    // Timer1.initialize(10000);        // 10ms
    Timer1.initialize(10000);
    Timer1.attachInterrupt(tickCountUp);
    
    // start Serial vis USB
    Serial.begin(115200);

    // output pin setup
    pinMode(PIN_LED, OUTPUT);

    for(int i=0; i<SEND_BUFFER_SIZE; i++){
        sendBuffer[i] = 'N';
    }

    for(int i=0; i<RECV_BUFFER_SIZE; i++){
        recvBuffer[i] = 'N';
    }

    initLoRa();
}

void loop(){
	unsigned long tcSendToActionCopy, tcRequestToActionCopy, tcSendToP5Copy;

//    noInterrupts();
//    // tickCountCopy = tickCount;
//    tcSendToActionCopy = tcSendToAction;
//    tcRequestToActionCopy = tcRequestToAction;
//    tcSendToP5Copy = tcSendToP5;
//    interrupts();

    if(tcSendToAction> 2) {  bSendToAction = true; }  // 20ms
    if(tcRequestToAction>20){ bRequestToAction = true; } // 100ms
    if(tcSendToP5>5){ bSendToP5 = true; } // 50ms

    updateBtnStatus();
    updateSendBuffer(bRequestToAction);
    sendToActionModule(bSendToAction);
    requestToActionModule(bRequestToAction);
    

//    getTempFromActionModule();
//     printBtnStatus();

//    printTemp();
    Serial.println(tcRequestToAction);
}

void initLoRa(){
    Serial.println("Feather LoRa TX Test!");

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

void updateBtnStatus(){
    for(int i=0; i<OUT_MESSAGE_SIZE; i++){
        inputBtnStatus[i] = true;  
    }
}

void printBtnStatus(){  // message
    for(int i=0; i<SEND_BUFFER_SIZE; i++){
        Serial.print((char)sendBuffer[i]);
        Serial.print(", ");
    }   
    Serial.println();
}

void updateSendBuffer(bool _bRequestToAction){
    /*
        sendBuffer[] = { /, B, 1, 0, 1, 0, 1, 1, 1, 1...} 
    */
    
    sendBuffer[0] = '/';
    sendBuffer[1] = 'B';
    for(int i=0; i<OUT_MESSAGE_SIZE; i++){
        if(inputBtnStatus[i])   sendBuffer[i+2] = '1';
        else                    sendBuffer[i+2] = '0';
    }
    

    if(bRequestToAction){
        sendBuffer[SEND_BUFFER_SIZE-4] = '/';
        sendBuffer[SEND_BUFFER_SIZE-3] = 'T';
        sendBuffer[SEND_BUFFER_SIZE-2] = '1';
        sendBuffer[SEND_BUFFER_SIZE-1] = 0;
    } else {
        sendBuffer[SEND_BUFFER_SIZE-4] = '/';
        sendBuffer[SEND_BUFFER_SIZE-3] = 'T';
        sendBuffer[SEND_BUFFER_SIZE-2] = '0';
        sendBuffer[SEND_BUFFER_SIZE-1] = 0;
    }
}

void sendToActionModule(bool _bSendToAction){
    if(_bSendToAction){
//        Serial.print("Action ");
        digitalWrite(PIN_LED, HIGH);
        
        rf95.send((uint8_t *)sendBuffer, SEND_BUFFER_SIZE);
        rf95.waitPacketSent();
        digitalWrite(PIN_LED, LOW);

        bSendToAction = false;
        tcSendToAction = 0;
    }
}

void requestToActionModule(bool _bRequestToAction){
    if(_bRequestToAction){
//        Serial.print("request ");
      
        digitalWrite(PIN_LED, HIGH);
//        char requestPacket[2] = "/T";
////        requestPacket[2] = 0;
//
//        rf95.send((uint8_t *)requestPacket, 2);

        //wait for a reply
        if(rf95.waitAvailableTimeout(1000)){
            uint8_t recvBufferLen = sizeof(recvBuffer);
    
            if(rf95.recv((char *)recvBuffer, &recvBufferLen)){
                // rf95.waitPacketSent();   // ??
                Serial.println("Received OK!");
            } else {
                // receive fail routine. kill switch?
                Serial.println("Receive failed");
            }
        } else {
            Serial.println("No reply..");
        }

        digitalWrite(PIN_LED, LOW);
        
        tcRequestToAction = 0;
        bRequestToAction = false;
        
    }
}


void getTempFromActionModule(){
    if(recvBuffer[0] == '/'){
        waterTemp.bin[0] = recvBuffer[1];
        waterTemp.bin[1] = recvBuffer[2];
        waterTemp.bin[2] = recvBuffer[3];
        waterTemp.bin[3] = recvBuffer[4];
        noodleTemp.bin[0] = recvBuffer[5];
        noodleTemp.bin[1] = recvBuffer[6];
        noodleTemp.bin[2] = recvBuffer[7];
        noodleTemp.bin[3] = recvBuffer[8];
    }
}

void printTemp(){
    Serial.print(waterTemp.floatPoint, 4);
    Serial.print(" : ");
    Serial.print(noodleTemp.floatPoint, 4);
    Serial.println();

    waterTemp.floatPoint = 0.f;
    noodleTemp.floatPoint = 0.1f;
}

void sendToP5(bool _bSendToP5){
    // TODO : send to Processing
    // start?
    // waterTemp, noodleTemp
    // pps update message
//     if( _bSendToP5){
//           Serial.print("P5 ");
//         if(digitalRead(PIN_START_BTN)){
//             Serial.println("S");
//         }
//         bSendToP5 = false;
//         tcSendToP5 = 0;
//     }
}
