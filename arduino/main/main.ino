#include <SPI.h>
#include "RF24.h"
#include <Stepper.h>
#include <ESP32Servo.h>

const int in1 = 32, in2 = 33, in3 = 25, in4 = 26; 
const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, 27, 12, 14, 13);

Servo cameraServo, headServo;
int headAngle = 90, cameraAngle = 20;

RF24 rf24(17,5);
const byte addr[] = "1Node";
int beforeTime = 0, waitTime = 100;

boolean isReload = false, isCharge = false;

void setup() {
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(2, OUTPUT); //LED comm status
    pinMode(22, OUTPUT); //relay
    pinMode(21, OUTPUT); //charge control
    pinMode(15, OUTPUT); //fire control

    digitalWrite(2, 0); //LED
    digitalWrite(in1, 0);
    digitalWrite(in2, 0);
    digitalWrite(in3, 0);
    digitalWrite(in4, 0);
    digitalWrite(22, 1);
    digitalWrite(21, 0);
    digitalWrite(15, 0);

    Serial.begin(115200);
    rf24.begin();
    rf24.setChannel(80);  // 設定頻道編號
    rf24.setPALevel(RF24_PA_MIN);
    rf24.setDataRate(RF24_2MBPS);
    rf24.openReadingPipe(1, addr);  // 開啟通道和位址
    rf24.startListening();  // 開始監聽無線廣播
    Serial.println("nRF24L01 ready!");

    cameraServo.attach(16);
    cameraServo.write(20);
    delay(200);
    cameraServo.detach();
    headServo.attach(4);
    headServo.write(90);
    delay(200);
    headServo.detach();
    myStepper.setSpeed(6);
}

void loop() {
    if(rf24.available()) {
        char msg[32];
        rf24.read(&msg, sizeof(msg));
        Serial.println(msg); // 顯示訊息內容
        operateCommand(msg);
        beforeTime = millis();
    }
    if(millis()-beforeTime >= waitTime){
        digitalWrite(2, 0);
        leftStop();
        rightStop();
        Serial.println("disconnect");
    }else{
        digitalWrite(2, 1);
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
        if(cameraAngle>=55) return;
        cameraServo.attach(16);
        cameraServo.write(++cameraAngle);
        delay(10);
        cameraServo.detach();
    }else if(strcmp(msg,"cameraDown") == 0){
        if(cameraAngle<=20) return;
        cameraServo.attach(16);
        cameraServo.write(--cameraAngle);
        delay(10);
        cameraServo.detach();
    }else if(strcmp(msg,"headLeft") == 0){
        if(headAngle>150) return;
        headServo.attach(4);
        headServo.write(++headAngle);
        delay(10);
        headServo.detach();
    }else if(strcmp(msg,"headRight") == 0){
        if(headAngle<30) return;
        headServo.attach(4);
        headServo.write(--headAngle);
        delay(10);
        headServo.detach();
    }else if(strcmp(msg,"leftRotate") == 0){
        myStepper.step(8);
    }else if(strcmp(msg,"rightRotate") == 0){
        myStepper.step(-8);
    }else if(strcmp(msg,"commCheck") == 0){
        digitalWrite(2, 1);
    }else if(strcmp(msg,"fire") == 0){
        leftStop();
        rightStop();
        delay(200);

        reload();
        recharge();
        fire();
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
    digitalWrite(in4, 0);
}
void reload(){
    Serial.println("reload");
    cameraServo.attach(16);
    cameraServo.write(20);
    delay(150);
    myStepper.step(stepsPerRevolution/32);
    delay(500);
    cameraServo.write(cameraAngle);
    delay(200);
    cameraServo.detach();
    isReload = true;
}
void recharge(){ 
    Serial.println("recharge");
    digitalWrite(22, 0);//relay on
    delay(1000);
    digitalWrite(21, 1); //start charge
    delay(100);
    digitalWrite(21, 0);
    delay(3000); //waiting charge
    
    isCharge = true;
    delay(100);
}
void fire(){
    if(!(isCharge && isReload)) return;
    Serial.println("fire");
    digitalWrite(15,1); //fire
    delay(40);
    digitalWrite(15,0);

    delay(200); //discharge 
    digitalWrite(15,1);
    delay(300);
    digitalWrite(15,0);

    digitalWrite(22, 1); //relay off
    isCharge = false;
    isReload = false;
    delay(100);
}