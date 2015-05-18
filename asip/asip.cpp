/*
 * asip.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


#include "asip.h"
#include "asipIO.h"

#ifdef ASIP_DEBUG
Stream *debugStream;   // debug output
#define DEBUG_TX_PIN 19   // connect TTL to USB adapter receive to this pin 
// set up a new serial port
SendOnlySoftwareSerial debugSoftSerial (DEBUG_TX_PIN);  
#endif

// message strings, Move this to program memory ?
char * errStr[] = {"NO_ERROR", "INVALID_SERVICE", "UNKNOWN_REQUEST", "INVALID_PIN", "MODE_UNAVAILABLE", "INVALID_MODE", "WRONG_MODE", "INVALID_DEVICE_NUMBER", "DEVICE_NOT_AVAILABLE", "I2C NOT ENABLED"};
 
asipClass:: asipClass(){
  
}
 
void asipClass::begin(Stream *s, int svcCount, asipServiceClass **serviceArray, char *sketchName )
{
  serial = s;
#ifdef ASIP_DEBUG  
  debugSoftSerial.begin(ASIP_DEBUG_BAUD);
  debugStream = &debugSoftSerial; 
  printf("\n"); // debug output 
  printf("ASIP %d.%d with sketch %s on %s\n", ASIP_MAJOR_VERSION, ASIP_MINOR_VERSION, sketchName, CHIP_NAME);
  VERBOSE_DEBUG( printf("Verbose Debug enabled\n");) // this will only print if VERBOSE_DEBUG macro argument is uncommented
#endif  
  services = serviceArray;
  nbrServices = svcCount; 
  // set all pins to UNALLOCATED_PIN state
  for(byte p=0; p < TOTAL_PINCOUNT; p++) { 
     setPinMode(p, UNALLOCATED_PIN_MODE);
  }
#ifdef ASIP_DEBUG   
  asip.reserve(DEBUG_TX_PIN); 
#endif  
  programName = sketchName;
  s->write(INFO_MSG_HEADER);
  s->print(sketchName);  
  // list all implemented service tags
  s->print(F(" running on "));
  s->print(F(CHIP_NAME));
  s->print(F(" with Services: "));
  
  for(int i=0; i < svcCount; i++ ){
    s->write(services[i]->ServiceId);
    s->write(' ');
  }
  s->write(MSG_TERMINATOR); 
}
 
void asipClass::service()
{ 
  if(serial->available() >= MIN_MSG_LEN) {  
     int tag = serial->read();     
     if( tag > ' ') { // ignore control characters          
        if(tag == SYSTEM_MSG_HEADER) {
          if(serial->read() == ',') {// tag must be followed by a separator 
              processSystemMsg();
           }
        }           
        else if(tag == INFO_MSG_HEADER) {
           processDebugMsg();        
        }        
        else {
          int svc = 0; 
          while(svc < nbrServices) {   
            if( services[svc]->ServiceId == tag) {
              if(serial->read() == ',') {// tag must be followed by a separator        
                services[svc]->processRequestMsg(serial);
               break;
              }
            }
           svc++;
           if(svc >= nbrServices) { // check if no match
             sendErrorMessage(tag, '?', ERR_INVALID_SERVICE, serial);         
            } 
          } 
        }           
     }
  }
  // service digital inputs
  sendDigitalPortChanges(serial, false);
  
  // auto events for services:
  unsigned int currentTick = millis();
  for(int i=0; i < nbrServices; i++) {
    if( services[i]->autoInterval > 0) {  // zero disables autoInterval
       //VERBOSE_DEBUG(  printf("Auto report, tick= %u, trig = %u, interval=%u\n",currentTick,(currentTick - services[i]->nextTrigger),services[i]->autoInterval);)
       if( currentTick >= services[i]->nextTrigger )  {
         services[i]->reportValues(serial);
         services[i]->nextTrigger =  currentTick + services[i]->autoInterval; // reset the count
        //VERBOSE_DEBUG( printf("Counter reset to %u\n", services[i]->nextTrigger);)
       }      
    }    
  }
}

void asipClass::processDebugMsg()
{   
    // echo incoming debug messages to the debug stream
    int c;
    unsigned int startMillis = millis();
    debugStream->write(INFO_MSG_HEADER);
    do {
      c = serial->read();
      if (c >= 0) {
        debugStream->write(c);
      }      
    } while( c != '\n' && millis() - startMillis < 1000); // wait at most one second for the end of the message        
}

void asipClass::processSystemMsg()
{
   int request = serial->read();   
   if(request == tag_SYSTEM_GET_INFO) {
      serial->write(EVENT_HEADER);   
      serial->write(SYSTEM_MSG_HEADER);
      serial->write(',');
      serial->write(tag_SYSTEM_GET_INFO);
      serial->write(',');
      serial->print(ASIP_MAJOR_VERSION);
      serial->write(',');
      serial->print(ASIP_MINOR_VERSION);
      serial->write(',');
      serial->print(CHIP_NAME);
      serial->write(',');
      serial->print(TOTAL_PINCOUNT);
      serial->write(',');
      serial->print(programName);
      serial->write(MSG_TERMINATOR);
   }
   else if(request == tag_SERVICES_NAMES) {
      // sends a list of service IDs and their friendly names
      serial->write(EVENT_HEADER);
      serial->write(SYSTEM_MSG_HEADER);
      serial->write(',');
      serial->write(tag_SERVICES_NAMES);
      serial->write(',');
      serial->print(nbrServices);
      serial->write(',');   
      serial->write('{');     
      for(byte i=0; i < nbrServices; i++) {
         char svcId = services[i]->ServiceId;
         if(isValidServiceId(svcId))  {
            serial->write( svcId ); 
            serial->write( ':' );
            services[i]->reportName(serial);                       
            if( i < nbrServices-1)
               serial->write(',');
         }  
       }
       serial->write('}');
       serial->write(MSG_TERMINATOR);
   }
   else if(request == tag_PIN_SERVICES_LIST) {
 // sends a list of all pins with associated service id if any
      serial->write(EVENT_HEADER);
      serial->write(SYSTEM_MSG_HEADER);
      serial->write(',');
      serial->write(tag_PIN_SERVICES_LIST);
      serial->write(',');
      serial->print(TOTAL_PINCOUNT);
      serial->write(','); 
      serial->write('{');
      for(byte p=0; p < TOTAL_PINCOUNT; p++) {    
         int svc = (char)getServiceId(p);
         serial->write( svc );          
         if( p != TOTAL_PINCOUNT-1)
            serial->write(',');
          else  
            serial->write('}');
      }
      serial->write(MSG_TERMINATOR);
   }   
   else if(request == tag_RESTART_REQUEST) {
       printf("Resetting services\n");
       for(int i=0; i < nbrServices; i++) {
           services[i]->reset();
           services[i]->autoInterval = 0;   // this disables autoInterval          
       }
   }
   else {
     sendErrorMessage(SYSTEM_MSG_HEADER, request, ERR_UNKNOWN_REQUEST, serial);
   }
}

// returns error code
asipErr_t asipClass::registerPinMode(byte pin, pinMode_t mode, char serviceId)
{
  asipErr_t err = ERR_NO_ERROR;
  // Serial.print("PIN in reg =") ; Serial.println(pin);    
  if(pin >= 0 && pin < TOTAL_PINCOUNT) {     
  // Serial.print("mode in reg =") ; Serial.println(pinModes[pin]); 
    // only system can set RESERVE_MODE
    if( (mode == RESERVED_MODE && serviceId == SYSTEM_SERVICE_ID) || isValidServiceId(serviceId) ){
        if( getPinMode(pin) < RESERVED_MODE) {    
          setPinMode(pin,mode); 
          pinRegister[pin].service = serviceId - '@';     
          VERBOSE_DEBUG(printf("register pin %d for mode %d for service %c (as %d)\n", pin, mode,serviceId,pinRegister[pin].service ));         
        }
        else {
         err = ERR_MODE_UNAVAILABLE;
        }    
    }
    else {
      err = ERR_INVALID_SERVICE;
    }
  }
  else {
     err = ERR_INVALID_PIN;
  }
  return err;
}

// only used for system reset requests
asipErr_t asipClass::deregisterPinMode(byte pin)
{
  asipErr_t err = ERR_NO_ERROR;  
  if(pin >= 0 && pin < TOTAL_PINCOUNT) {     
    if( getPinMode(pin) < RESERVED_MODE) {    
      setPinMode(pin,UNALLOCATED_PIN_MODE);  
      VERBOSE_DEBUG(printf("deregister  pin %d\n", pin));  
    }
    else {
     err = ERR_MODE_UNAVAILABLE;
    }    
  }
  else {
     err = ERR_INVALID_PIN;
  }
  if(err)  printf("de-register error %d\n", err);
  return err;
}

// reserve the given pin
asipErr_t asipClass::reserve(byte pin)
{
     VERBOSE_DEBUG(printf("Reserving pin %d\n", pin));  
     return registerPinMode(pin,RESERVED_MODE,SYSTEM_SERVICE_ID);
} 
 
bool asipClass::isValidServiceId(char serviceId)
{
  return (serviceId >= 'A' && serviceId <= 'Z');
}

asipServiceClass*  asipClass::serviceFromId( char tag)
{
   asipServiceClass* svcPtr = NULL;
    for(int svc=0; svc < nbrServices; svc++) {
         if( services[svc]->ServiceId == tag) {
            svcPtr = services[svc];
            break;
         }
    }
    return svcPtr;  
}   

// Sets the mode of the given pin 
void asipClass::setPinMode(byte pin, pinMode_t mode) 
{
  if( pin >=0 && pin < TOTAL_PINCOUNT) {
    //pinModes[pin] = mode;
    pinRegister[pin].mode = mode; 
  } 
}

// returns the mode of the given pin 
pinMode_t asipClass::getPinMode(byte pin) 
{
  if( pin >=0 && pin < TOTAL_PINCOUNT) {
    return (pinMode_t)pinRegister[pin].mode;
    //return pinModes[pin];
  }
  else {
    return INVALID_MODE;
  } 
}

// returns the service id associated with the given pin 
char asipClass::getServiceId(byte pin) 
{
  char svc = '?';
  if( pin >=0 && pin < TOTAL_PINCOUNT) {
    
    if( pinRegister[pin].mode == OTHER_SERVICE_MODE){
       svc =  (char) pinRegister[pin].service + '@';
     }
    else if( pinRegister[pin].mode == RESERVED_MODE) {
      svc = '@';      
    }   
    else
      svc = id_IO_SERVICE;
  }         
   return svc;  
}

 
void asipClass::sendPinModes()  // sends a list of all pin modes
{
  serial->write(EVENT_HEADER);
  serial->write(id_IO_SERVICE);
  serial->write(',');
  serial->write(tag_PIN_MODES);
  serial->write(',');
  serial->print(TOTAL_PINCOUNT);
  serial->write(',');  // comma added 21 June 2014
  serial->write('{');
  for(byte p=0; p < TOTAL_PINCOUNT; p++) {
     //int mode = (char)pinModes[p]; 
     int mode = (char)getPinMode(p);
     serial->print( mode); 
     if( p != TOTAL_PINCOUNT-1)
        serial->write(',');
      else  
        serial->write('}');
  }
  serial->write(MSG_TERMINATOR); 
} 

void asipClass::sendPinCapabilites()  // sends a bitfield array indicating capabilities all pins 
{
  capabilityMask mask;
  
  serial->write(EVENT_HEADER);
  serial->write(id_IO_SERVICE);
  serial->write(',');
  serial->write(tag_PIN_CAPABILITIES);
  serial->write(',');
  serial->print(TOTAL_PINCOUNT); 
  serial->write(',');  // comma added 21 June 2014
  serial->write('{');
  for(byte p=0; p < TOTAL_PINCOUNT; p++) {
     mask.ch = 0; // clear the mask
     IS_PIN_DIGITAL(p) ? mask.bits.DIGITAL_IO = 1 : mask.bits.DIGITAL_IO = 0;
     IS_PIN_ANALOG(p) ? mask.bits.ANALOG_INPUT = 1 :  mask.bits.ANALOG_INPUT = 0;
     IS_PIN_PWM(p)    ? mask.bits.PWM_OUTPUT = 1 : mask.bits.PWM_OUTPUT = 0 ;    
     serial->write( mask.ch + '0'); // convert to a printable character 
     if( p != TOTAL_PINCOUNT-1)
        serial->write(',');
      else  
        serial->write('}');
  }
  serial->write(MSG_TERMINATOR); 
} 

void asipClass::sendPortMap()
{
byte port,mask;
  // note that port numbers may not start at 0 and may not be consecutive
  // pins that have no digital capability have port and mask values of 0 
  serial->write(EVENT_HEADER);
  serial->write(id_IO_SERVICE);
  serial->write(',');
  serial->write(tag_GET_PORT_TO_PIN_MAPPING);
  serial->write(',');
  serial->print(TOTAL_PINCOUNT);
  serial->write(',');  // comma added 21 June 2014
  serial->write('{');
  for(byte p=0; p < TOTAL_PINCOUNT; p++) {
    if(IS_PIN_DIGITAL(p)) {
       port = DIGITAL_PIN_TO_PORT(p);
       mask = DIGITAL_PIN_TO_MASK(p);
    }   
    else {
       port = mask = 0;
    }
     serial->print(port); // port number sent as decimal
     serial->write(':');
     serial->print(mask, HEX); // note the mask is sent as Hex
     if( p != TOTAL_PINCOUNT-1)
        serial->write(',');
      else  
        serial->write('}');
  }
  serial->write(MSG_TERMINATOR); 
}

void asipClass::sendAnalogPinMap()
{
  int pinsToReport = TOTAL_ANALOG_PINS;
  // sends pairs of digital:analog pin associations
  serial->write(EVENT_HEADER);
  serial->write(id_IO_SERVICE);
  serial->write(',');
  serial->write(tag_GET_ANALOG_PIN_MAPPING);
  serial->write(',');
  serial->print(TOTAL_ANALOG_PINS);
  serial->write(','); 
  serial->write('{');
  for(byte p=0; p < TOTAL_PINCOUNT; p++) {
    if(IS_PIN_ANALOG(p)) {
      serial->print(p);
      serial->write(':');
      serial->print(PIN_TO_ANALOG(p)); 
     if( --pinsToReport > 0)
        serial->write(',');
      else  
        serial->write('}');
    }
  } 
  serial->write(MSG_TERMINATOR); 
  if( pinsToReport != 0) {
     printf("number of analog pins is off by %d\n", pinsToReport);
  }
}

void asipClass::sendErrorMessage( const char svc, const char tag, asipErr_t errno, Stream *stream)
{
  stream->write(ERROR_MSG_HEADER);
  stream->write(svc);
  stream->write(',');
  stream->write(tag);  
  stream->write(',');  
  stream->print(errno);
  stream->print('{');  
  stream->print(errStr[errno]); 
  stream->write('}');
  serial->write(MSG_TERMINATOR);   
} 

asipClass asip;

asipServiceClass::asipServiceClass(const char svcId, const char evtId) :
   ServiceId(svcId), EventId(evtId) 
{
}

asipServiceClass::asipServiceClass(const char svcId) :
   ServiceId(svcId), EventId(tag_SERVICE_EVENT) 
{
}

void asipServiceClass::begin(byte _nbrElements, byte _pinCount, const pinArray_t _pins[])
{
  nbrElements =  _nbrElements;
  pinCount = _pinCount;
  pins = _pins;
  autoInterval = 0; // turn off auto events
  for( byte p=0; p <pinCount; p++) {
     asip.registerPinMode(pins[p], OTHER_SERVICE_MODE,ServiceId);
  } 
}

void asipServiceClass::begin(byte nbrElements, const pinArray_t pins[])
{
  begin(nbrElements, nbrElements, pins);
}
  
void asipServiceClass::begin(byte _nbrElements, serviceBeginCallback_t serviceBeginCallback) // begin with no pins starts an I2C service
{
  nbrElements =  _nbrElements;
  autoInterval = 0; // turn off auto events
  if(serviceBeginCallback != NULL) {
    if( serviceBeginCallback(ServiceId) == false) {
       // service failed to start
    }   
  }  
}

// can be invoked by clients to restore conditions to start-up state
 void asipServiceClass::reset()
 {
 
 }
 
asipServiceClass::~asipServiceClass(){} 

void asipServiceClass::reportValues(Stream *stream) 
{
  stream->write(EVENT_HEADER);
  stream->write(ServiceId);
  stream->write(',');
  stream->write(EventId);
  stream->write(',');
  stream->print(nbrElements);
  stream->write(',');
  stream->write('{');
  for(byte count = 0; count < nbrElements; count++){   
      reportValue(count, stream);
      if(count < nbrElements-1)
         stream->write(',');  // comma between all but last element
  }
  stream->write('}');
  stream->write(MSG_TERMINATOR); 
  
}

void asipServiceClass::setAutoreport(Stream *stream) // reads stream and sets number of ticks between events 
{
  unsigned int ticks = stream->parseInt();
  setAutoreport(ticks);
}

void asipServiceClass::setAutoreport(unsigned int ticks) // sets number ticks between events, 0 disables 
{
  autoInterval = ticks;
  unsigned int currentTick = millis(); // truncate to a 16 bit value
  nextTrigger = currentTick + autoInterval; // set the next trigger tick count
  printf("\n"); //(todo - the first character printed to softserial here is corrupted)
  printf("Auto report set to %u for service %c\n",ticks, ServiceId);
}

char asipServiceClass::getServiceId()
{
  return ServiceId;
}

void asipServiceClass::reportName(Stream *stream)
{
  PGM_P name  = this->svcName;
  for (uint8_t c; (c = pgm_read_byte(name)); name++) {
    stream->write(c); 
  }
}

void asipServiceClass::reportError( const char svc, const char request, asipErr_t errno, Stream *stream)
{
   asip.sendErrorMessage(svc,request, errno, stream);
}
