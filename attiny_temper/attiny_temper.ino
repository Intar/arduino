#define nRepeatTransmit 10
#define nPulseLength 350
#define Pin 3
#define Temp 4
void setup() {
  enableTransmit(Pin);
  pinMode(Temp,INPUT);
}

void enableTransmit(int nTransmitterPin) {
  nTransmitterPin = nTransmitterPin;
  pinMode(nTransmitterPin, OUTPUT);
}


void loop() {
  int temp = 0;
  for (int i = 0;i<6;i++){
     temp = temp + analogRead(Temp);
  }
  temp = temp/6;
  for (int i = 0; i<50;i++){
    send(temp, 20);
  }
 // delay(100);  
}
void send(unsigned long code, unsigned int length) {
  sends(dec2binWzerofill(code, length));
}

void sends(char* sCodeWord) {
  for (int nRepeat=0; nRepeat<nRepeatTransmit; nRepeat++) {
    int i = 0;
    while (sCodeWord[i] != '\0') {
      switch(sCodeWord[i]) {
        case '0':
        transmit(1,3);
        break;
        case '1':
        transmit(3,1);
        break;
      }
      i++;
    }
    transmit(1,31);
  }
}


void transmit(int nHighPulses, int nLowPulses) {
      digitalWrite(Pin, HIGH);
      delayMicroseconds(nPulseLength * nHighPulses);
      digitalWrite(Pin, LOW);
      delayMicroseconds(nPulseLength * nLowPulses);
}


char* dec2binWzerofill(unsigned long Dec, unsigned int bitLength){
    return dec2binWcharfill(Dec, bitLength, '0');
}

char* dec2binWcharfill(unsigned long Dec, unsigned int bitLength, char fill){
  static char bin[64];
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : fill;
    Dec = Dec >> 1;
  }
  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    }else {
      bin[j] = fill;
    }
  }
  bin[bitLength] = '\0';
  
  return bin;
}

