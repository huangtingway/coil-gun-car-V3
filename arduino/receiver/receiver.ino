#include <SPI.h>
#include "RF24.h"
//#include <SoftwareSerial.h>

RF24 rf24(9,10);
const byte addr[] = "1Node";
const char startmsg[] = "start comm";
boolean isConnect = false;

enum XState {normalX, left, right};
enum YState {normalY, front, back};
enum XState xState;
enum YState yState;

short cameraAngleCounter = 0;
short headAngleCounter = 0;

void getDirection();
void sendDirection();
void sendNRFData();

void setup() {
    Serial.begin(115200);
    rf24.begin();
    rf24.setChannel(80); // 設定頻道編號
    rf24.openWritingPipe(addr); // 設定通道位址
    rf24.setPALevel(RF24_PA_MIN);
    rf24.setDataRate(RF24_2MBPS);
    rf24.stopListening();       // 停止偵聽；設定成發射模式
    sendNRFData("startComm");

    pinMode(2,INPUT);
    pinMode(3,INPUT);
    pinMode(4,INPUT);
    pinMode(5,INPUT);
    pinMode(6,INPUT);
    pinMode(7,INPUT);
    pinMode(8,INPUT);
    pinMode(A0,INPUT);
    pinMode(A1,INPUT);
}

void loop() {
    getDirection();
    sendDirection();

    if(digitalRead(2) == 0){
        cameraAngleCounter++;
        if(cameraAngleCounter%10 == 0) sendNRFData("cameraUp");
    }
    if(digitalRead(3)== 0){
        headAngleCounter--;
        if(headAngleCounter%10 == 0) sendNRFData("headRight");
    }
    if(digitalRead(4)== 0){
        cameraAngleCounter--;
        if(cameraAngleCounter%10 == 0) sendNRFData("cameraDown");
    }
    if(digitalRead(5)== 0){
        headAngleCounter++;
        if(headAngleCounter%10 == 0) sendNRFData("headLeft");
    }
    if(digitalRead(6)== 0) sendNRFData("leftRotate");
    if(digitalRead(7)== 0) sendNRFData("rightRotate");
    if(digitalRead(8)== 0) Serial.println("main btn");
}

void getDirection(){
  if(analogRead(A0)>900) xState = right;
  else if(analogRead(A0)<250) xState = left;
  else xState = normalX;

  if(analogRead(A1)>800) yState = front;
  else if(analogRead(A1)<250) yState = back;
  else yState = normalY;
}

void sendDirection(){
    switch (yState) {
        case front:
            switch (xState) {
                case left:
                    sendNRFData("front_left");
                    break;
                case right:
                    sendNRFData("front_right");
                    break;
                case normalX:
                    sendNRFData("front");
                    break;
            }
        break;
        case back:
            switch (xState) {
                case left:
                    sendNRFData("back_left");
                    break;
                case right:
                    sendNRFData("back_right");
                    break;
                case normalX:
                    sendNRFData("back");
                    break;
            }
            break;
        case normalY:
            switch (xState) {
                case left:
                    sendNRFData("left");
                    break;
                case right:
                    sendNRFData("right");
                    break;
                case normalX:
                    sendNRFData("stop");
                    break;
            }
            break;
    }
}

void sendNRFData(String getMsg){
    int str_len = getMsg.length() + 1; 
    char sendmsg[str_len];
    getMsg.toCharArray(sendmsg,str_len); 
    rf24.write(&sendmsg, sizeof(sendmsg));
    Serial.println(sendmsg);
}
