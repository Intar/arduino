#include <Arduino.h>
#include <util.h>
#include <LiquidCrystal.h>
#include <RCSwitch.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
RTC_DS1307 RTC;
// Arduino pins
#define Relay 3
#define key 300000
#define buttonUp 9
#define buttonDown 12
#define backlight 8
// initialize the library with the number of the sspin
//(or the latch pin of the 74HC595)
LiquidCrystal lcd(10);

//int hours ;
//const int TEMP_DAY = 22;
//const int TEMP_WORK = 17;
//const int TEMP_NIGHT = 21;
RCSwitch mySwitch = RCSwitch();
float temp_home = 0;
unsigned long nextqueryTime = 0;
int buttonState = 1;
int buttonVal = 0;
int temp = 22;
int temper = 20;
void setup()
{
  Serial.begin(9600);
  pinMode(Relay, OUTPUT);
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(backlight, OUTPUT);
  mySwitch.enableReceive(0);
  lcd.begin(16,2);
  LCDUpdateTemp(temp);
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT Running");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  }
 
void loop ()
{
  DateTime now = RTC.now();
  buttonVal = digitalRead(buttonUp);
 if(buttonVal==LOW){
   temper++;
   digitalWrite(backlight, LOW);
   check_temp(temp,temper);
   LCDUpdateConf(temper);
/*Example for date   Serial.println(__DATE__);
   Serial.println(now.year(), DEC);
   Serial.println(now.month(), DEC);
   Serial.println(now.day(), DEC);
   Serial.println(now.hour(), DEC);
   Serial.println(get_day_of_week(now.dayOfWeek()));
*/
   delay(200);
   digitalWrite(backlight, HIGH);
 }
 buttonVal = digitalRead(buttonDown);
  if(buttonVal==LOW){
   temper--;
   digitalWrite(backlight, LOW);
   check_temp(temp,temper);
   LCDUpdateConf(temper);
   delay(200);
   digitalWrite(backlight, HIGH);
 }

//if (millis()>= nextqueryTime){
  if (mySwitch.available()) {
    for (int i = 0;i<6;i++){
      float value = mySwitch.getReceivedValue()*5.0/1024*100;
      Serial.println(value);
      if (value == 0){
        i--;
      }
     temp_home = temp_home + value;
     Serial.println(temp_home);
                            }
    temp_home = temp_home/6.0;
    Serial.println(temp_home);
    temp_home = temp_home + 0.5;//для перевода в инт, иначе 23.9 переведется как 23
    temp = (int) temp_home;
    check_temp(temp,temper);
    temp_home = 0;
    Serial.println(temp);
    LCDUpdateTemp(temp);
    mySwitch.resetAvailable();
//    nextqueryTime = millis() + 1500;
                         } 
  //                      }  
}

void LCDUpdateTemp(int temp)
{
        lcd.setCursor(0,0);
        lcd.print("HomeTemp:");
        lcd.print(temp);
        lcd.print("C");
}

void LCDUpdateConf(int temper)
{
  lcd.setCursor(0,1);
  lcd.print("ConfTemp:");
  lcd.print(temper);
  lcd.print("C");
}

String get_day_of_week(uint8_t dow) {
  String dows= " ";
  switch(dow){
    case 0: dows="Su"; break;
    case 1: dows="Mo"; break;
    case 2: dows="Tu"; break;
    case 3: dows="We"; break;
    case 4: dows="Th"; break;
    case 5: dows="Fr"; break;
    case 6: dows="Sa"; break;
  }
  return dows;
}

void check_temp (int current, int conf)
{
  if (current <= conf - 1) {
      Serial.println("Relay on temp:");
      Serial.print(current);
      Serial.print(conf);
      RelayStat(1);
        }
      else if (current >= conf + 1) { 
              Serial.println("Relay off temp:");
      Serial.print(current);
      Serial.print(conf);

        RelayStat(0);}          
}    
void RelayStat(int stat)
{
  switch (stat){
    case 1:
      digitalWrite(Relay, LOW);
      break;
    case 0:
      digitalWrite(Relay, HIGH);
      break; 
  }
}


