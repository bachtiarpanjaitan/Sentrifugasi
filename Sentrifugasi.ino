#include <Bataxdevino.h>
//#include <Keypad.h>
#include <SoftwareSerial.h>

const int RS = 8;
const int EN = 9;
const int d4 = 4;
const int d5 = 5;
const int d6 = 6;
const int d7 = 7;
const int BL = 10;
const int buttonPin = 0;
const int motor = 53;
const int pinSensor = 52;
//const byte numRows= 4;
//const byte numCols= 4;
//
//char* keymap[numRows][numCols]= {
//  {'1', '2', '3', 'A'},
//  {'4', '5', '6', 'B'},
//  {'7', '8', '9', 'C'},
//  {'*', '0', '#', 'D'}
//};

//byte rowPins[numRows] = {22,24,26,28}; //Rows 0 to 3
//byte colPins[numCols]= {30,32,34,36}; //Columns 0 to 3

//Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

char* menuText[] = {"TIMER","START"};
int menuSize = 1;
int menuPosition = 0;
int menuLevel = 0;
float timerValue = 0; // satuan detik
char timerValueString[3];
float RPM = 0;
float putaran = 0;
int timeStart = 0;
int timeEnd = 0;
int pengaliWaktu = 1000; //1000 jika detik; 60000 jika menit
String bluetoothData = "";

LiquidCrystal lcd (RS,EN,d4,d5,d6,d7);
SoftwareSerial bluetoothSerial(50,51);
Bataxdevino btx;
void setup() {
  lcd.begin(16,2);
  Serial.begin(9600);
  bluetoothSerial.begin(38400);
  btx.lcdPrint(lcd,0,0,"SELECT MENU",true);

  pinMode(pinSensor, INPUT);
  pinMode(motor,OUTPUT);
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
  digitalWrite(motor,HIGH);
  delay(1000);
    timeStart = millis();
    int timerInMilliSecond = timerValue * pengaliWaktu;
    do {
        if(digitalRead(pinSensor) == LOW) putaran += 1;
        delay(1);
        timeEnd  = millis() - timeStart;
       // Serial.println(putaran);
      }while(timeEnd < timerInMilliSecond);
      
    float radian = putaran / (44/7) ;
    if(pengaliWaktu == 1000){
      float x = (float)timerValue / (float)60;
      RPM = radian/x;
    }else RPM = radian/timerValue;
    delay(500);
    Serial.println(RPM);
    timerValue = 0;
    digitalWrite(motor,LOW);

   char* rpm = strcat(btx.intToChar(RPM)," RPM");
   btx.lcdPrint(lcd,0,0,rpm,true);
}


void loop() {
  
//  Serial.println(menuLevel);
  
 if(btx.readLcdButton(buttonPin) == 1 && menuLevel == 0){
    if(menuPosition > 0){
      menuPosition -= 1;
      delay(150);
    }
    menu();
    Serial.println("ATAS");
  }
  else if(btx.readLcdButton(buttonPin) == 2 && menuLevel == 0){
    if(menuPosition < 2){
      menuPosition += 1;
      delay(150);
    }
    menu();
    Serial.println("BAWAH");
  }

  
  if(btx.readLcdButton(buttonPin) == 3 && menuPosition == 0 && menuLevel == 1){
    if(menuPosition == 0 && timerValue != 0){
      char* x = strcat(btx.intToChar(timerValue)," SECOND");
      //char* text = strcat("TIMER :",x);
      btx.lcdPrint(lcd,0,0,x,true);
      menuLevel = 0;
      menuPosition = 0;
    }
  }
  
  if(btx.readLcdButton(buttonPin) == 4 && menuPosition == 1 && menuLevel == 0){
    Serial.println(btx.readLcdButton(buttonPin));
    if(timerValue == 0 ){
       btx.lcdPrint(lcd,1,0,"SET TIMER FIRST");
    }else startTimer();
    
  }
  
  if(btx.readLcdButton(buttonPin) == 4){
    if( menuPosition == 0 && menuLevel == 0){
      Serial.println("TIMER MENU");
      menuLevel = 1;
    }
  }
  //Event menu timer
  if(menuPosition == 0 && menuLevel == 1 ){
      if(btx.readLcdButton(buttonPin) == 1) timerValue += 1;
      if(btx.readLcdButton(buttonPin) == 2) timerValue -= 1;
      btx.lcdPrint(lcd,0,0,"SET TIMER",true);
      char* text = strcat(btx.intToChar(timerValue)," M");
      btx.lcdPrint(lcd,1,0,text);
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
    //Serial.println(bluetoothData);
    if(bluetoothData != "")inputDelimeter(bluetoothData);
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
