#include <util.h>
#include <EthernetUdp.h>
#include <Ethernet.h>
#include <RCSwitch.h>
#include <SPI.h>
#define Relay 4
#define key 300000
byte mac[]= {
  0xDE, 0xAD, 0xEF, 0xFE, 0xED};
byte ip[] = {192, 168, 106, 197 };
byte dns_serv[] = { 192, 168, 109, 252 };
byte gateway[] = { 192, 168, 106, 254 }; 
unsigned int localPort = 8888;
int hours ;
IPAddress timeServer (192, 168, 109, 252);
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];
EthernetUDP Udp;
const int TEMP_DAY = 22;
const int TEMP_WORK = 17;
const int TEMP_NIGHT = 21;
RCSwitch mySwitch = RCSwitch();
float temp_home;
unsigned long queryTime = 0;
unsigned long nextqueryTime = 0;

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip, dns_serv, gateway);
  Udp.begin(localPort);
  pinMode(Relay, OUTPUT);
  mySwitch.enableReceive(0);
 }
 
void loop ()
{
      unsigned long receivedCode= mySwitch.getReceivedValue();
  Serial.println(receivedCode);
  if (millis()>= nextqueryTime){
  if (mySwitch.available()) {
    unsigned long receivedCode= mySwitch.getReceivedValue();
      if (receivedCode == 0) {
        Serial.println("ErroR");
      } else {
        //Serial.println(receivedCode);
        temp_home = (float(receivedCode) - 500.0 - (float)key)/100.0;
        Serial.print("Temperature: ");
        Serial.print(temp_home);
      }
      mySwitch.resetAvailable();
  } else { Serial.println("Not receive value"); }
  sendNTPpacket(timeServer);
  Serial.println(nextqueryTime);
  nextqueryTime = millis() + 15000;
      // wait to see if a reply is available
  //delay(1000);  
  if ( Udp.parsePacket() ) {  
    // We've received a packet, read the data from it
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;  
   // Serial.print("Seconds since Jan 1 1900 = " );
    //Serial.println(secsSince1900);               

    // now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;     
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;  
    // print Unix time:
    //Serial.println(epoch);                               


    // print the hour, minute and second:
    Serial.print("The GMT time is ");       // UTC is the time at Greenwich Meridian (GMT)
    hours = (epoch  % 86400L) / 3600 + 4; //hours in GMT+4
    if (hours>=24) hours= hours - 24;
    Serial.print(hours); // print the hour (86400 equals secs per day)
    Serial.print(':');  
    if ( ((epoch % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':'); 
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch %60); // print the second
  }
  }
  // wait ten seconds before asking for the time again
//  RelayStat(1);
//  Serial.println("Relay on");
//  delay(10000);
//  RelayStat(0); 
//  Serial.println("Relay off");
 delay(7000);
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

// send an NTP request to the time server at the given address 
unsigned long sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:         
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
} 
