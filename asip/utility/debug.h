/*
 debug.h 
 
 Debug printing is enabled by defining ASIP_DEBUG in this file and
 using debug_printf statements in your code 
 More extensive debug is enabled by also defining ASIP_VERBOSE_DEBUG and 
 using verbose_printf statements and defining ASIP_VERBOSE_DEBUG in this file.

debug_printf and verbose_printf use standard printf arguments with syntax 
dependant on the compiler used.
By default, line length is limited to 80 chars on 8 bit MCUs
 
 Output can be sent to the same stream as ASIP, on another stream that can be hardware or 
 software serial.
  to use software Serial, define ASIP_DEBUG_SOFT_SERIAL and set the pin in asip.cpp using the txPin define.
 Baud rate is set with the ASIP_DEBUG_BAUD, 57600 is the highest rate reliably supported by 8MHz boards
*/

 /*
Send only software serial routines are adapted from SoftwareSerial by Nick Gammon 28th June 2012

This code is derived from SoftwareSerial.h (formerly NewSoftSerial.h) - 
Multi-instance software serial library for Arduino/Wiring
-- Interrupt-driven receive and other improvements by ladyada
   (http://ladyada.net)
-- Tuning, circular buffer, derivation from class Print/Stream,
   multi-instance support, porting to 8MHz processors,
   various optimizations, PROGMEM delay tables, inverse logic and 
   direct port writing by Mikal Hart (http://www.arduiniana.org)
-- Pin change interrupt macros by Paul Stoffregen (http://www.pjrc.com)
-- 20MHz processor support by Garrett Mace (http://www.macetech.com)
-- ATmega1280/2560 support by Brett Hagman (http://www.roguerobotics.com/)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef asip_debug_h
#define asip_debug_h

#include <inttypes.h>
#include <Stream.h>

extern  Stream *debugStream;   // debug output

#define ASIP_DEBUG
#define debugStream  (&Serial)
//#define ASIP_VERBOSE_DEBUG
#define ASIP_DEBUG_BAUD 57600  // 57600 is highest rate for 8MHz boards 
#define ASIP_DEBUG_BUFFER_SIZE 80 // max length of a debug string

#if defined (ASIP_VERBOSE_DEBUG) && defined(ASIP_DEBUG)
#define verbose_printf(...) debug_printf(...) 
#else 
#define verbose_printf(...)
#endif

#ifdef ASIP_DEBUG
//#define ASIP_DEBUG_SOFT_SERIAL

#if defined(__MK20DX256__) // Teensy 3.1
#define debug_printf                     \
    debugStream->write(DEBUG_MSG_INDICATOR); \
    debugStream->printf
#else  // 8 bit MCUs assume no built-in  printf support
extern char _buf[];  // for printf
#define debug_printf(...)                         \
    debugStream->write(DEBUG_MSG_INDICATOR); \
    do {                            \
        sprintf(_buf, __VA_ARGS__); debugStream->print(_buf); \
    } while (0) 
#endif // check for teensy or other chip    
#else // #ASIP_DEBUG not defined
#define debug_printf(...) 
#endif // PRINTF_DEBUG


/******************************************************************************
* Definitions
******************************************************************************/

#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

class SendOnlySoftwareSerial : public Stream
{
private:
  // per object data
  uint8_t _transmitBitMask;
  volatile uint8_t *_transmitPortRegister;

  uint16_t _tx_delay;

  uint16_t _inverse_logic;

  // private methods
  void tx_pin_write(uint8_t pin_state);
  void setTX(uint8_t transmitPin);

  // private static method for timing
  static inline void tunedDelay(uint16_t delay);

public:
  // public methods
  SendOnlySoftwareSerial(uint8_t transmitPin, bool inverse_logic = false);
  ~SendOnlySoftwareSerial();
  void begin(long speed);
  void end();
  int peek();

  virtual size_t write(uint8_t byte);
  virtual int read();
  virtual int available();
  virtual void flush();
  
  using Print::write;
};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round

#endif  // SendOnlySoftwareSerial_h
