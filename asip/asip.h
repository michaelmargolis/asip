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

//#define PRINTF_DEBUG
#define VERBOSE_DEBUG(X) //X  // uncomment x to enable verbose debug


/* Version numbers for the protocol.  
 * This number can be queried so that host software can test
 *  whether it will be compatible with the installed firmware. 
 */ 
const int ASIP_MAJOR_VERSION  = 0; // for non-compatible changes
const int ASIP_MINOR_VERSION  = 1; // for backwards compatibility

// error messages
enum asipErr_t {ERR_NO_ERROR, ERR_INVALID_SERVICE, ERR_UNKNOWN_REQUEST, ERR_INVALID_PIN, ERR_MODE_UNAVAILABLE,
                ERR_INVALID_MODE, ERR_WRONG_MODE, ERR_INVALID_DEVICE_NUMBER, ERR_DEVICE_NOT_AVAILABLE, ERR_I2C_NOT_ENABLED};
                
typedef byte pinArray_t; // the type used by services to provide an array of needed pins 
typedef bool (*serviceBeginCallback_t)(const char svc);   // callback for services such as I2C that don't explicitly use pins

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
const char SYSTEM_MSG_HEADER      = '#';  // system requests are preceded with this tag
const char tag_SYSTEM_GET_INFO    = '?';  // Get version and hardware info
const char tag_RESTART_REQUEST    = 'R';  // disables all autoevents and attempts to restart all services 

// messages from Arduino
const char EVENT_HEADER        = '@';  // event messages are preceded with this tag 
const char ERROR_MSG_HEADER    = '~';  // error messages begin with this tag
const char DEBUG_MSG_HEADER    = '!';  // debug messages begin with this tag

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


#ifdef PRINTF_DEBUG
extern char _buf[64];
#define printf(...)                         \
    Serial.write(DEBUG_MSG_HEADER);   \
    do {                            \
        sprintf(_buf, __VA_ARGS__); Serial.write(_buf); \
    } while (0) 
    
 #else
 #define printf(...) 
 #endif

class asipServiceClass 
{
public:
  asipServiceClass(const char svcId); // Unique service ID and use of default system event tag
  asipServiceClass(const char svcId, const char evtId); // use this if you need an event tag different from default
  virtual ~asipServiceClass();   
  virtual void begin(byte nbrElements, const pinArray_t pins[]);  // you can use this terse version when there is one pin per element
  virtual void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]); // specify total number of pins
  virtual void begin(byte nbrElements, serviceBeginCallback_t serviceBeginCallback); // begin with no pins starts an I2C service
  virtual void reset()=0;                                   // can be invoked by clients to restore conditions to start-up state
  virtual void reportValue(int sequenceId, Stream * stream)  = 0; // send the value of the given device
  virtual void reportValues(Stream *stream); // send all values separated by commas, preceded by header and terminated with newline
  virtual void setAutoreport(Stream *stream); // how many ticks between events, 0 disables 
  virtual void processRequestMsg(Stream *stream) = 0;
  virtual void reportError( const char svc, const char request, asipErr_t errno, Stream *stream); // report service request errors
  virtual void reportName(Stream *stream);
  virtual char getServiceId();  
  PGM_P svcName;
  
protected:
   const char EventId;         // the unique character that identifies the default event provided by service
   byte nbrElements;           // the number of items supported by this service
   byte pinCount;              // total number of pins in the pins array 
   const pinArray_t *pins;     // stores pins used by this service  
   
   friend class asipClass; 
   const char ServiceId;       // the unique Upper Case ASCII character that identifies this service 
   unsigned int autoInterval;  // the number of ticks between each autoevent, 0 disables autoevents
   unsigned int nextTrigger;   // tick value for the next event (note this rolls over after 65 seconds so intervals should be limited to under one minute  
};

class asipClass 
{
public:
  asipClass();
  void begin(Stream *s, int svcCount, asipServiceClass *serviceArray[], char *sketchName );
  asipErr_t registerPinMode(byte pin, pinMode_t mode, char serviceId);
  asipErr_t reserve(byte pin); 
  asipErr_t reserveSerialPins();
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

  Stream *serial;
  char *programName;
  asipServiceClass **services;
  int nbrServices; 
  pinRegistration_t pinRegister[TOTAL_PINCOUNT];
  boolean I2C_Started;

  void processSystemMsg();
  bool isValidServiceId(char serviceId);
  asipServiceClass* serviceFromId( char tag);

 };
 
extern asipClass asip;

#endif