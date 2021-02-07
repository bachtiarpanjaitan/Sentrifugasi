#include <Bataxdevino.h>
//#include <Keypad.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Drive.h>  //Include the Drive library

const int RS = 8;
const int EN = 9;
const int d4 = 4;
const int d5 = 5;
const int d6 = 6;
const int d7 = 7;
const int BL = 10;
const int buttonPin = 0;

const int pinSensor = 52;
const int in1 = 53;
const int in2 = 49;
const int enA = A8;

char* menuText[] = {"TIMER","START"};
int menuSize = 1;
int menuPosition = 0;
int menuLevel = 0;
float timerValue = 0; // satuan detik
char timerValueString[3];
float RPM = 0;
float putaran = 0;
float timeStart = 0;
float timeEnd = 0;
float pengaliWaktu = 60000; //1000 jika detik; 60000 jika menit
String bluetoothData = "";
int speedMotor = 100; // 0-255

LiquidCrystal lcd (RS,EN,d4,d5,d6,d7);
SoftwareSerial bluetoothSerial(50,51);

const int IN1 = 30;
const int IN2 = 31;
const int IN3 = 32;
const int IN4 = 33;
const int POT = A9;

Drive drive(IN1, IN2, IN3, IN4);

Bataxdevino btx;
void setup() {
  lcd.begin(16,2);
  Serial.begin(9600);
  bluetoothSerial.begin(38400);
  btx.lcdPrint(lcd,0,0,"SELECT MENU",true);

  pinMode(pinSensor, INPUT);
  pinMode(enA,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(POT, INPUT);

    digitalWrite(in1,HIGH);
    digitalWrite(in2,HIGH);
}

void menu(){
  if(menuPosition > menuSize){
    menuPosition  = 1;
  }
  if(menuPosition < 0)menuPosition = 0;
  btx.lcdPrint(lcd,0,0,"------MENU------",true);
  btx.lcdPrint(lcd,1,0,menuText[menuPosition]);
}

void startTimer(){
  digitalWrite(in1,LOW);
  putaran = 0;
  delay(1000);    
    timeStart = millis();
    float timerInMilliSecond = timerValue * pengaliWaktu;
    do {
        if(digitalRead(pinSensor) == HIGH) putaran += 1;
        delay(1);
        timeEnd  = millis() - timeStart;
       int npot = analogRead(POT);
       drive.moveForward(npot/4);
      }while(timeEnd < timerInMilliSecond);
      
    float radian = putaran / (44/7) ;
    if(pengaliWaktu == 1000){
      float x = (float)timerValue / (float)60;
      RPM = radian/x;
    }else RPM = radian/timerValue;
    delay(500);
    //Serial.println(RPM);
    timerValue = 0;
    digitalWrite(in1,HIGH);

   char* rpm = strcat(btx.intToChar(RPM)," RPM");
   btx.lcdPrint(lcd,0,0,rpm,true);
}


void loop() {
  
//  Serial.println(menuLevel);
  analogWrite(enA, speedMotor);

 if(btx.readLcdButton(buttonPin) == 1 && menuLevel == 0){
    if(menuPosition > 0){
      menuPosition -= 1;
      delay(150);
    }
    menu();
  }
  else if(btx.readLcdButton(buttonPin) == 2 && menuLevel == 0){
    if(menuPosition < 2){
      menuPosition += 1;
      delay(150);
    }
    menu();
  }

  
  if(btx.readLcdButton(buttonPin) == 3 && menuPosition == 0 && menuLevel == 1){
    if(menuPosition == 0 && timerValue != 0){
      char* x = strcat(btx.intToChar(timerValue)," MNT");
      //char* text = strcat("TIMER :",x);
      btx.lcdPrint(lcd,0,0,x,true);
      menuLevel = 0;
      menuPosition = 0;
    }
    
  }
  
  if(btx.readLcdButton(buttonPin) == 4 && menuPosition == 1 && menuLevel == 0){
    if(timerValue == 0 ){
       btx.lcdPrint(lcd,1,0,"SET TIMER FIRST");
    }else startTimer();
    
  }
  
  if(btx.readLcdButton(buttonPin) == 4){
    if( menuPosition == 0 && menuLevel == 0){
      menuLevel = 1;
    }
  }
  //Event menu timer
  if(menuPosition == 0 && menuLevel == 1 ){
      if(btx.readLcdButton(buttonPin) == 1) timerValue += 1;
      if(btx.readLcdButton(buttonPin) == 2) timerValue -= 1;
      btx.lcdPrint(lcd,0,0,"SET TIMER",true);
      char* text = strcat(btx.intToChar(timerValue)," MNT");
      btx.lcdPrint(lcd,0,10,text);
      int pot = (int) analogRead(POT)/4;
      char* c_pot = btx.intToChar(pot); 
      btx.lcdPrint(lcd,1,0,"SPEED : ",false);
      btx.lcdPrint(lcd,1,8,c_pot,false);
    }

   if(RPM != 0 && timerValue !=0){
     char* rpm = strcat(btx.intToChar(RPM)," RPM");
     btx.lcdPrint(lcd,0,0,rpm,true);
     RPM = 0;
   }

   if(bluetoothSerial.available()){
      delay(2);
      char dataSerial = bluetoothSerial.read();
      bluetoothData += dataSerial;
   }

    
   

   if(!bluetoothSerial.available()){
//    Serial.println(bluetoothData);
    if(bluetoothData != ""){
      //inputDelimeter(bluetoothData);
      timerValue = bluetoothData.toInt();
    }
    bluetoothData = "";
   }
   
  delay(150);
  //Serial.print(menuLevel);
  //Serial.println(menuPosition);
}

void inputDelimeter(String data){
  for (int i = 0; i < data.length(); i++) {
    if (data.substring(i, i+1) == "|") {
      timerValue = (float) data.substring(0, i).toInt();
      pengaliWaktu=(int) data.substring(i+1).toInt();
      break;
    }
  }
}
