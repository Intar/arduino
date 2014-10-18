#include <RCSwitch.h>
#define Temp 0
#define Relay 4
#define key 300000
float temp;
float value;
RCSwitch mySwitch = RCSwitch(); 
void setup(){
  pinMode(Temp,INPUT);
  Serial.begin(9600);
  mySwitch.enableTransmit(8);
  pinMode(Relay, OUTPUT);
}
void loop(){
  temp = 0;
  for (int i = 0;i<6;i++){
    value = analogRead(Temp)*5.0/1024*100;
    temp = temp + value;
  }
  temp = temp/6;
  Serial.println(temp);
  temp = temp*100+key+500;
  Serial.println(temp);
  sendRC(temp);
 // delay(60000);
}

void sendRC(unsigned long code) {
  mySwitch.send(code, 20);
}
