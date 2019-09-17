/* TinyMegaI2C

   David Johnson-Davies - www.technoblogy.com - 17th September 2019
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

#include "TinyMegaI2CMaster.h"

TinyMegaI2CMaster::TinyMegaI2CMaster() 
{
}

// Minimal Tiny I2C Routines **********************************************

void TinyMegaI2CMaster::init () {
  pinMode(PIN_WIRE_SDA, INPUT_PULLUP);
  pinMode(PIN_WIRE_SCL, INPUT_PULLUP);
  uint32_t baud = ((F_CPU_CORRECTED/FREQUENCY) - (((F_CPU_CORRECTED*T_RISE)/1000)/1000)/1000 - 10)/2;
  TWI0.MBAUD = (uint8_t)baud;
  TWI0.MCTRLA = TWI_ENABLE_bm;                                        // Enable as master, no interrupts
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

uint8_t TinyMegaI2CMaster::read (void) {
  if (I2Ccount != 0) I2Ccount--;
  while (!(TWI0.MSTATUS & TWI_RIF_bm));                               // Wait for read interrupt flag
  uint8_t data = TWI0.MDATA;
  // Check slave sent ACK?
  if (I2Ccount != 0) TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;             // ACK = more bytes to read
  else TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_RECVTRANS_gc;           // Send NAK
  return data;
}

uint8_t TinyMegaI2CMaster::readLast (void) {
  I2Ccount = 0;
  return TinyMegaI2CMaster::read();
}

bool TinyMegaI2CMaster::write (uint8_t data) {
  while (!(TWI0.MSTATUS & TWI_WIF_bm));                               // Wait for write interrupt flag
  TWI0.MDATA = data;
  TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;                                // Do nothing
  return !(TWI0.MSTATUS & TWI_RXACK_bm);                              // Returns true if slave gave an ACK
}

// Start transmission by sending address
bool TinyMegaI2CMaster::start (uint8_t address, int readcount) {
  bool read;
  if (readcount == 0) read = 0;                                       // Write
  else { I2Ccount = readcount; read = 1; }                            // Read
  TWI0.MADDR = address<<1 | read;                                     // Send START condition
  while (!(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm)));                // Wait for write or read interrupt flag
  if ((TWI0.MSTATUS & TWI_ARBLOST_bm)) return false;                  // Return false if arbitration lost or bus error
  return !(TWI0.MSTATUS & TWI_RXACK_bm);                              // Return true if slave gave an ACK
}

bool TinyMegaI2CMaster::restart(uint8_t address, int readcount) {
  return TinyMegaI2CMaster::start(address, readcount);
}

void TinyMegaI2CMaster::stop (void) {
  TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;                     // Send STOP
}

TinyMegaI2CMaster TinyMegaI2C = TinyMegaI2CMaster();                  // Instantiate a TinyMegaI2C object
