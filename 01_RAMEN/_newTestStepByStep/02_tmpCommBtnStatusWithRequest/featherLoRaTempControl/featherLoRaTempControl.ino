#include <SPI.h>
#include <RH_RF95.h>

const int PIN_LED         = 13;

const int RFM95_CS      = 8;
const int RFM95_RST     = 4;
const int RFM95_INT     = 7;

const float RF95_FREQ   = 433.0;

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
bool btnStatus[10];



RH_RF95 rf95(RFM95_CS, RFM95_INT);
unsigned long countTick;


void setup(){
    Serial.begin(115200);
    pinMode(PIN_LED, OUTPUT);

    delay(1000);
    initLoRa();
    countTick = 0;

    for(int i=0; i<10; i++){
        if(i%2 == 0)    btnStatus[i] = true;
        else            btnStatus[i] = false;
    }
}

void loop(){
    // delay(1000);
    
    countTick++;

    
    if(countTick > 200){
       requestTempToActionModule();
        // sendBtnStatus();
        countTick = 0;
        // delay(100);
    } 
    else {
       sendBtnStatus();
       // delay(100);
    }

    Serial.println(countTick);
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
    Serial.println("btn sent!");
    char sendBtnPacket[sizeof(btnStatus) + 3];

    sendBtnPacket[0] = '/';
    sendBtnPacket[1] = 'B';
    for(int i=0; i<sizeof(btnStatus); i++){
        if(btnStatus[i])    sendBtnPacket[i+2] = '1';
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







