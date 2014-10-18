/*
LCD3Wire v0.1 14/Feb/2007 ahoeben http://hoeben.net/aldo
         v0.2 21/Jun/2011 Paul Cunnane - inherit from Print class

What is this?
An arduino library for comms with HD44780-compatible LCD, using a 4094 shiftregister and only 3 pins on the Arduino

Sources:
- The original "LiquidCrystal" 8-bit library and tutorial
    http://www.arduino.cc/en/uploads/Tutorial/LiquidCrystal.zip
    http://www.arduino.cc/en/Tutorial/LCDLibrary
- neillzero's LCD4Bit library
    http://www.arduino.cc/playground/Code/LCD4BitLibrary
- Dojodave's LCD3wires instructions and tutorial
    http://www.arduino.cc/playground/Code/LCD3wires
- DEM 16216 datasheet http://www.maplin.co.uk/Media/PDFs/N27AZ.pdf

- Massimo's suggested 4-bit code (I took initialization from here) http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1144924220/8
See also:
- glasspusher's code (probably more correct): http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1160586800/0#0

Tested only with a DEM 16216 (maplin "N27AZ" - http://www.maplin.co.uk/Search.aspx?criteria=N27AZ)
If you use this successfully, consider feeding back to the arduino wiki with a note of which LCD it worked on.

Usage:
see the examples folder of this library distribution.

*/
#include "Arduino.h"
#include "LCD3Wire.h"
extern "C" {
  #include <string.h> //needed for strlen()
//  #include "WConstants.h"  //all things wiring / arduino
}


//command bytes for LCD
#define CMD_CLR   0x01
#define CMD_RIGHT 0x1C
#define CMD_LEFT  0x18
#define CMD_HOME  0x02

// bitmasks for control bits on shift register
#define SHIFT_EN B00010000
#define SHIFT_RW B00100000
#define SHIFT_RS B01000000


// --------- settings -------------------------------------

//Data, Clock and Strobe pins are set using the constructor
int dat_pin;
int str_pin;
int clk_pin;

//number of lines of the LCD, set using the constructor
int lcd_lines;



//stuff the library user might call---------------------------------
//constructor.  num_lines must be 1 or 2, currently.
LCD3Wire::LCD3Wire (int _lcd_lines, int _dat_pin, int _str_pin, int _clk_pin) {
  lcd_lines = _lcd_lines;
  if (lcd_lines < 1 || lcd_lines > 2)
    lcd_lines = 1;
 
  dat_pin = _dat_pin;
  str_pin = _str_pin;
  clk_pin = _clk_pin;
}

// initiatize lcd after a short pause
//while there are hard-coded details here of lines, cursor and blink settings, you can override these original settings after calling .init()
void LCD3Wire::init () {
  // initialize pins
  pinMode(dat_pin,OUTPUT);
  pinMode(str_pin,OUTPUT);
  pinMode(clk_pin,OUTPUT);
  
  // initiatize lcd after a short pause
  // needed by the LCDs controller
  // just in case init() is the very first thing that happens in the program
  delay(100);
  
  /////////// 4 pin initialization
  commandWrite(0x03); // function set: 4 pin initialization
  commandWrite(0x03); // function set: 4 pin initialization
  commandWrite(0x03); // function set: 4 pin initialization
  commandWrite(0x02); // function set: 4 pin initialization
  if (lcd_lines==1)
    commandWrite(0x20); // function set: 4-bit interface, 1 display line, 5x7 font
  else
    commandWrite(0x28); // function set: 4-bit interface, 2 display lines, 5x7 font
  /////////// end of 4 pin initialization 
  
  commandWrite(0x06); // entry mode set:
  // increment automatically, no display shift
  commandWrite(0x0e); // display control:
  // turn display on, cursor on, no blinking
  commandWrite(0x01); 
  // clear display, set cursor position to zero
}

void LCD3Wire::commandWrite(int value) {
  _pushByte(value, true);
  delay(5);
}

//print the given character at the current cursor position. overwrites, doesn't insert.
inline size_t LCD3Wire::write(uint8_t value) {
  _pushByte(value, false);
}

void LCD3Wire::_pushByte(int value, bool command) {
  int nibble = 0;

  digitalWrite(str_pin,LOW); // set the strobe LOW

  nibble = value >> 4; //send the first 4 databits (from 8)
  _pushNibble(nibble, command);

  delay(1);

  nibble = value & 15; // set HByte to zero 
  _pushNibble(nibble, command);
}

// called twice by each _pushByte
void LCD3Wire::_pushNibble(int nibble, bool command) {
  if (!command) {
      nibble |= SHIFT_RS; // set DI HIGH
      nibble &= ~SHIFT_RW; // set RW LOW
  }
  nibble &= ~SHIFT_EN; // set Enable LOW
  _pushOut(nibble);
  nibble |= SHIFT_EN; // Set Enable HIGH
  _pushOut(nibble);
  nibble &= ~SHIFT_EN; // set Enable LOW
  _pushOut(nibble); 
}

// push byte to shift register and on to LCD
void LCD3Wire::_pushOut(int value) {
  shiftOut(dat_pin, clk_pin, LSBFIRST, value);
  digitalWrite(str_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(str_pin, LOW);
}





//non-core stuff --------------------------------------

//send the clear screen command to the LCD
void LCD3Wire::clear(){
  commandWrite(CMD_CLR);
}

//move the cursor to the given absolute position.  line numbers start at 1.
//if this is not a 2-line LCD3Wire instance, will always position on first line.
void LCD3Wire::cursorTo(int line_num, int x){
  //first, put cursor home
  commandWrite(CMD_HOME);

  //if we are on a 1-line display, set line_num to 1st line, regardless of given
  if (lcd_lines==1){
    line_num = 1;
  }
  //offset 40 chars in if second line requested
  if (line_num == 2){
    x += 40;
  }
  //advance the cursor to the right according to position. (second line starts at position 40).
  for (int i=0; i<x; i++) {
    commandWrite(0x14);
  }
}

//scroll whole display to left
void LCD3Wire::leftScroll(int num_chars, int delay_time){
  for (int i=0; i<num_chars; i++) {
    commandWrite(CMD_LEFT);
    delay(delay_time);
  }
}
