/* Digital Clock using TinyMegaI2C Library

   David Johnson-Davies - www.technoblogy.com - 6th June 2018
   ATtiny402 @ 20 MHz (internal oscillator; BOD disabled)
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

#include <TinyMegaI2CMaster.h>

// Digital clock **********************************************

const int RTCAddress = 0x68;
const int DisplayAddress = 0x70;
int Colon = 2;

char Segment[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void SetClock (int hr, int min) {
  TinyMegaI2C.start(RTCAddress, 0);
  TinyMegaI2C.write(0);
  TinyMegaI2C.write(0);
  TinyMegaI2C.write(min);
  TinyMegaI2C.write(hr);
  TinyMegaI2C.stop();
}

void InitDisplay () {
  TinyMegaI2C.start(DisplayAddress, 0);
  TinyMegaI2C.write(0x21);
  TinyMegaI2C.restart(DisplayAddress, 0);
  TinyMegaI2C.write(0x81);
  TinyMegaI2C.restart(DisplayAddress, 0); 
  TinyMegaI2C.write(0xe1);
  TinyMegaI2C.stop();
}

void WriteWord (uint8_t b) {
  TinyMegaI2C.write(b);
  TinyMegaI2C.write(0);
}

void WriteTime (uint8_t hrs, uint8_t mins) {
  TinyMegaI2C.start(DisplayAddress, 0);
  TinyMegaI2C.write(0);
  WriteWord(Segment[hrs / 16]);
  WriteWord(Segment[hrs % 16]);
  WriteWord(Colon);
  WriteWord(Segment[mins / 16]);
  WriteWord(Segment[mins % 16]);
  TinyMegaI2C.stop();
}
  
// Setup **********************************************

void setup() {
  TinyMegaI2C.init();
  InitDisplay();
  SetClock(0x12, 0x34);      // Set the time to 12:34 - change to current time
}

void loop () {
 // Read the time from the RTC
  TinyMegaI2C.start(RTCAddress, 0);
  TinyMegaI2C.write(1);
  TinyMegaI2C.restart(RTCAddress, 2);
  int mins = TinyMegaI2C.read();
  int hrs = TinyMegaI2C.read();
  TinyMegaI2C.stop();
  
  // Write the time to the display
  WriteTime(hrs, mins);
  
  // Flash the colon
  Colon = 2 - Colon;
  delay(1000);
}