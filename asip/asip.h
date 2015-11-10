
/*
 * asip.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef asip_h
#define asip_h

#include "boards.h"  // Hardware pin macros
#include "Arduino.h"
#include "asipService.h"
#include "utility/debug.h"

//#define ASIP_DEBUG             // define this to print debug info to the software serial stream 

/* Version numbers for the protocol.  
 * This number can be queried so that host software can test
 *  whether it will be compatible with the installed firmware. 
 */ 
const int ASIP_MAJOR_VERSION  = 0; // for non-compatible changes
const int ASIP_MINOR_VERSION  = 4; // for backwards compatibility

const long ASIP_BAUD  = 57600; // baud rate for the serial link     
const long DEBUG_BAUD = 57600; // baud rate for the debug stream


#define prog_char  char PROGMEM  // for deprecated defines - TODO replace these !!!
                
enum pinMode_t {UNALLOCATED_PIN_MODE, INPUT_MODE, INPUT_PULLUP_MODE, OUTPUT_MODE, ANALOG_MODE, PWM_MODE, RESERVED_MODE, OTHER_SERVICE_MODE, INVALID_MODE };

// bit field indicating the capability of pins (see asipIO for usage)
struct pinCapability_t {
   byte DIGITAL_IO    : 1;   
   byte ANALOG_INPUT  : 1;   
   byte PWM_OUTPUT    : 1;    
   //byte ANALOG_OUTPUT : 1; //DAC not supported on 8 bit chips
   };

union capabilityMask
  {
    pinCapability_t bits;
    char ch;
  } ; 

struct pinRegistration_t  {
     byte mode    : 3;
     byte service : 5;
  };   
   
const char SYSTEM_SERVICE_ID     = '@'; // only used when de-registering pins at reset    
//System messages
// Request messages to Arduino
const char SYSTEM_MSG_HEADER       = '#';  // system requests are preceded with this tag
const char tag_SYSTEM_GET_INFO     = '?';  // Get version and hardware info
const char tag_SERVICES_NAMES      = 'N';  // get list of friendly service names 
const char tag_PIN_SERVICES_LIST   = 'S';  // gets a list of pins indicating registered service 
const char tag_RESTART_REQUEST     = 'R';  // disables all autoevents and attempts to restart all services
  
// messages from Arduino
const char EVENT_HEADER        = '@';  // event messages are preceded with this tag 
const char ERROR_MSG_HEADER    = '~';  // error messages begin with this tag
const char INFO_MSG_HEADER     = '!';  // info messages begin with this tag
const char DEBUG_MSG_INDICATOR = '!';  // debug text within info messages are preceded with this tag

// tags available to all services (Don’t use these for some other service specific function)
const char tag_AUTOEVENT_REQUEST = 'A';  // this tag sets autoevent status
const char tag_REMAP_PIN_REQUEST = 'M';  // for services that can change pin numbers
// Reply tags common to all services
const char tag_SERVICE_EVENT     = 'e';  //  

const byte MIN_MSG_LEN = 4;  // valid request messages must be at least this many characters

const char NO_EVENT = '\0';  // tag to indicate the a service does not produce an event
const char MSG_TERMINATOR = '\n';

#define asipCHECK_PINS(a) extern const pinArray_t (a); //a macro to test if number of pins is same as server instances
//#define asipSvcName   PROGMEM const prog_char * 
#define asipSvcName static PROGMEM const prog_char 

#define asipServiceCount(s)  (sizeof(s) / sizeof(asipService))

class asipClass 
{
public:
  asipClass();
  //void begin(Stream *s, int svcCount, asipServiceClass *serviceArray[], char *sketchName );
  void begin(Stream *s, int svcCount, asipServiceClass (**serviceArray), char const *sketchName );
  asipErr_t registerPinMode(byte pin, pinMode_t mode, char serviceId);
  asipErr_t reserve(byte pin); 
  void service();
  void sendPortMap(); 
  void sendAnalogPinMap();
  void sendPinModes(); 
  void sendErrorMessage( const char svc, const char tag, asipErr_t errno, Stream *stream); 
private:
  friend class asipIOClass; 
  // low level interface 
  void sendAnalog(byte pin, int value);
  void sendDigitalPort(byte portNumber, int portData);
  asipErr_t deregisterPinMode(byte pin);
  pinMode_t getPinMode(byte pin); 
  char getServiceId(byte pin); 
  void setPinMode(byte pin, pinMode_t mode); 
  void sendPinCapabilites();
  void sendPinServicesList();

  Stream *serial;        // the link
  char *programName;
  asipServiceClass **services;
  int nbrServices; 
  pinRegistration_t pinRegister[TOTAL_PINCOUNT];
  boolean I2C_Started;

  void processSystemMsg();
  void processDebugMsg();
  bool isValidServiceId(char serviceId);
  asipServiceClass* serviceFromId( char tag);

 };
 
extern asipClass asip;

#endif