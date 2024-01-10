#include <SPI.h>
#include "RF24.h"
#include <Stepper.h>
#include <ESP32Servo.h>

const int in1 = 32, in2 = 33, in3 = 25, in4 = 26; 
const int stepsPerRevolution = 2048;
Servo cameraServo, headServo;
Stepper myStepper(stepsPerRevolution, 27, 12, 14, 13);

RF24 rf24(17,5);
const byte addr[] = "1Node";

void setup() {
    Serial.begin(115200);
    rf24.begin();
    rf24.setChannel(80);  // 設定頻道編號
    rf24.setPALevel(RF24_PA_MIN);
    rf24.setDataRate(RF24_2MBPS);
    rf24.openReadingPipe(1, addr);  // 開啟通道和位址
    rf24.startListening();  // 開始監聽無線廣播
    Serial.println("nRF24L01 ready!");

    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(2, OUTPUT); //LED comm status

    digitalWrite(2, 0); //LED
    digitalWrite(in1, 0);
    digitalWrite(in2, 0);
    digitalWrite(in3, 0);
    digitalWrite(in4, 0);

    cameraServo.attach(16);
    cameraServo.write(0);
    delay(100);
    cameraServo.detach();
    headServo.attach(4);
    headServo.write(0);
    delay(100);
    headServo.detach();

    myStepper.setSpeed(6);
}

void loop() {
  if (rf24.available()) {
        char msg[32];
        rf24.read(&msg, sizeof(msg));
        Serial.println(msg); // 顯示訊息內容
        //operateCommand(msg);
    }
}

void operateCommand(char msg[32]){
    if(strcmp(msg,"front") == 0){
        leftUp();
        rightUp();
    }else if(strcmp(msg,"back") == 0){
        leftDown();
        rightDown();
    }else if(strcmp(msg,"left") == 0){
        leftDown();
        rightUp();
    }else if(strcmp(msg,"right") == 0){
        leftUp();
        rightDown();
    }else if(strcmp(msg,"front_left") == 0){
        leftStop();
        rightUp();
    }else if(strcmp(msg,"front_right") == 0){
        leftUp();
        rightStop();
    }else if(strcmp(msg,"back_left") == 0){
        leftStop();
        rightDown();
    }else if(strcmp(msg,"back_right") == 0){
        leftDown();
        rightStop();
    }else if(strcmp(msg,"stop") == 0){
        leftStop();
        rightStop();
    }else if(strcmp(msg,"cameraUp") == 0){
        
    }else if(strcmp(msg,"cameraDown") == 0){
        
    }else if(strcmp(msg,"cameraStop") == 0){
        
    }else if(strcmp(msg,"leftRotate") == 0){
        myStepper.step(32);
    }else if(strcmp(msg,"rightRotate") == 0){
        myStepper.step(-32);
    }else if(strcmp(msg,"startComm") == 0){
        digitalWrite(2, 1);
    }
}

void leftUp(){
    digitalWrite(in1, 1);
    digitalWrite(in2, 0);
}
void leftDown(){
    digitalWrite(in1, 0);
    digitalWrite(in2, 1);
}
void leftStop(){
    digitalWrite(in1, 0);
    digitalWrite(in2, 0);
}
void rightUp(){
    digitalWrite(in3, 1);
    digitalWrite(in4, 0);
}
void rightDown(){
    digitalWrite(in3, 0);
    digitalWrite(in4, 1);
}
void rightStop(){
    digitalWrite(in3, 0);
    digitalWrite(in4, 1);
}