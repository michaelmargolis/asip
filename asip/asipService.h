/*
 * asipService.h -  Base class for ASIP services
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef asipService_h
#define asipService_h
                  
typedef bool (*serviceBeginCallback_t)(const char svc);   // callback for services such as I2C that don't explicitly use pins
typedef byte pinArray_t; // the type used by services to provide an array of needed pins 

// error messages
enum asipErr_t {ERR_NO_ERROR, ERR_INVALID_SERVICE, ERR_UNKNOWN_REQUEST, ERR_INVALID_PIN, ERR_MODE_UNAVAILABLE,
                ERR_INVALID_MODE, ERR_WRONG_MODE, ERR_INVALID_DEVICE_NUMBER, ERR_DEVICE_NOT_AVAILABLE, ERR_I2C_NOT_ENABLED};
                
#define asipSvcName static PROGMEM const prog_char 

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
   void setAutoreport(unsigned int ticks); // sets number ticks between events, 0 disables 
   const char EventId;         // the unique character that identifies the default event provided by service
   byte nbrElements;           // the number of items supported by this service
   byte pinCount;              // total number of pins in the pins array 
   const pinArray_t *pins;     // stores pins used by this service  
   
   friend class asipClass; 
   const char ServiceId;       // the unique Upper Case ASCII character that identifies this service 
   unsigned int autoInterval;  // the number of ticks between each autoevent, 0 disables autoevents
   unsigned int nextTrigger;   // tick value for the next event (note this rolls over after 65 seconds so intervals should be limited to under one minute
};

typedef asipServiceClass* asipService;

#define asipServiceCount(s)  (sizeof(s) / sizeof(asipService))

#endif
