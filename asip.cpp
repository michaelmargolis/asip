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

#ifdef PRINTF_DEBUG
char _buf[64];
#endif

// message strings, Move this to program memory ?
char * errStr[] = {"NO_ERROR", "INVALID_SERVICE", "UNKNOWN_REQUEST", "INVALID_PIN", "MODE_UNAVAILABLE", "INVALID_MODE", "WRONG_MODE", "INVALID_DEVICE_NUMBER", "DEVICE_NOT_AVAILABLE", "I2C NOT ENABLED"};
 
asipClass:: asipClass(){
  
}
 
 
void asipClass::begin(Stream *s, int svcCount, asipServiceClass **serviceArray, char *sketchName )
{
  serial = s;
  services = serviceArray;
  nbrServices = svcCount; 
  autoEventTickDuration = 1; // one millisecond between event counter ticks (TODO?)
  // set all pins to UNALLOCATED_PIN state
  for(byte p=0; p < NUM_DIGITAL_PINS; p++) {
     pinModes[p] = UNALLOCATED_PIN_MODE;
  }
   programName = sketchName;
  s->write(DEBUG_MSG_HEADER);
  s->print(sketchName);  
  // list all implemented service tags
  s->print(" running on ");
  s->print(CHIP_NAME);
  s->print(" with Services: ");
  
  for(int i=0; i < svcCount; i++ ){
    s->write(services[i]->ServiceId);
    s->write(' ');
  }
  s->println();  
 
}

void asipClass::service()
{ 
  if(serial->available() >= MIN_MSG_LEN) {  
     int tag = serial->read();
     if( tag != '\n') { // for now, ignore the newline at the end of the message   
        if(tag == SYSTEM_MSG_HEADER) {
          if(serial->read() == ',') {// tag must be followed by a separator 
              processSystemMsg();
           }
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
  sendDigitalPortChanges(serial);
  
  // auto events for services:
  unsigned int currentTick = millis();
  if(currentTick - previousTick > autoEventTickDuration) {
    previousTick += autoEventTickDuration;
    for(int i=0; i < nbrServices; i++) {
      if( services[i]->autoInterval > 0) {  // zero disables autoInterval
          //VERBOSE_DEBUG(  printf("Auto report, tick= %u, trig = %u, interval=%u\n",currentTick,(currentTick - services[i]->nextTrigger),services[i]->autoInterval);)
         if(services[i]->nextTrigger - currentTick >= services[i]->autoInterval) {
           services[i]->reportValues(serial);
           services[i]->nextTrigger =  currentTick + services[i]->autoInterval; // reset the count
           //VERBOSE_DEBUG( printf("Counter reset to %u\n", services[i]->nextTrigger);) 
         }      
      }
    }
  }
}

void asipClass::processSystemMsg()
{
   int request = serial->read();
   if(request == SYSTEM_GET_INFO) {
      serial->write(EVENT_HEADER);   
      serial->write(SYSTEM_MSG_HEADER);
      serial->write(',');
      serial->write(SYSTEM_GET_INFO);
      serial->write(',');
      serial->print(ASIP_MAJOR_VERSION);
      serial->write(',');
      serial->print(ASIP_MINOR_VERSION);
      serial->write(',');
      serial->print(CHIP_NAME);
      serial->write(',');
      serial->println(programName);
   }
   else {
     sendErrorMessage(SYSTEM_MSG_HEADER, request, ERR_UNKNOWN_REQUEST, serial);
   }
}

// returns error code
asipErr_t asipClass::registerPinMode(byte pin, pinMode_t mode)
{
  asipErr_t err = ERR_NO_ERROR;
  // Serial.print("PIN in reg =") ; Serial.println(pin);    
  if(pin >= 0 && pin < NUM_DIGITAL_PINS) {     
  // Serial.print("mode in reg =") ; Serial.println(pinModes[pin]); 
    if( pinModes[pin] != RESERVED_MODE && pinModes[pin] != OTHER_SERVICE_MODE) {    
      pinModes[pin] = mode;  
      //Serial.print("register ");Serial.print(pin); Serial.print(" for mode "); Serial.println(mode);         
    }
    else {
     err = ERR_MODE_UNAVAILABLE;
    }    
  }
  else {
     err = ERR_INVALID_PIN;
  }
  return err;
}

// returns them mode of the given pin 
pinMode_t asipClass::getPinMode(byte pin) 
{
  if( pin >=0 && pin < NUM_DIGITAL_PINS) {
    return pinModes[pin];
  }
  else {
    return INVALID_MODE;
  }
 
}

 
void asipClass::sendPinModes()  // sends a list of all pin modes
{
  serial->write(EVENT_HEADER);
  serial->write(IO_SERVICE);
  serial->write(',');
  serial->write(GET_PIN_MODES);
  serial->write(',');
  serial->print(NUM_DIGITAL_PINS);
  serial->write('{');
  for(byte p=0; p < NUM_DIGITAL_PINS; p++) {
     int mode = (char)pinModes[p]; // print values  > 127 as negative numbers
     serial->print( mode); 
     if( p != NUM_DIGITAL_PINS-1)
        serial->write(',');
      else  
        serial->println("}");
  }
} 

void asipClass::sendPinMap()
{
  // note that port numbers do not start at 0 and may not be consecutive
  serial->write(EVENT_HEADER);
  serial->write(IO_SERVICE);
  serial->write(',');
  serial->write(GET_PORT_TO_PIN_MAPPING);
  serial->write(',');
  serial->print(NUM_DIGITAL_PINS);
  serial->write('{');
  for(byte p=0; p < NUM_DIGITAL_PINS; p++) {
     byte port = digitalPinToPort(p);
     byte mask = digitalPinToBitMask(p);
     serial->print(port);
     serial->write(':');
     serial->print(mask, HEX); // note the mask is sent as Hex
     if( p != NUM_DIGITAL_PINS-1)
        serial->write(',');
      else  
        serial->println("}");
  } 
}

void asipClass::sendErrorMessage( const char svc, const char tag, asipErr_t errno, Stream *stream)
{
  stream->write(ERROR_MSG_HEADER);
  stream->write(svc);
  stream->write(',');
  stream->write(tag);  
  stream->write(':');  
  stream->print(errno);
  stream->print('(');  
  stream->print(errStr[errno]); 
  stream->println(')');  
  
} 

void asipClass::startI2C()
{
#if defined USE_I2C
  if(I2C_Started == false) {  
     Wire.begin();
     I2C_Started = true;
  }
#else
  sendErrorMessage(SYSTEM_MSG_HEADER,SYSTEM_MSG_HEADER,ERR_I2C_NOT_ENABLED, serial);
#endif  
}


asipClass asip;


asipServiceClass::asipServiceClass(const char svcId, const char evtId) :
   ServiceId(svcId), EventId(evtId) 
{

}

void asipServiceClass::begin(byte _nbrElements, byte _pinCount, const pinArray_t _pins[])
{
  nbrElements =  _nbrElements;
  pinCount = _pinCount;
  pins = _pins;
  autoInterval = 0; // turn off auto events
  for( byte p=0; p <pinCount; p++) {
     asip.registerPinMode(pins[p], OTHER_SERVICE_MODE);
  }
  //printf(%d, svc begin\n",ServiceId);
}

void asipServiceClass::begin(byte _nbrElements) // begin with no pins starts an I2C service
{
  nbrElements =  _nbrElements;
  autoInterval = 0; // turn off auto events
  asip.startI2C();
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
  for(byte count = 0; count < nbrElements; count++){
      stream->write(',');    
      reportValue(count, stream);
  }
  stream->write('\n'); // todo - make define
  
}

void asipServiceClass::setAutoreport(Stream *stream) // reads stream for number of ticks between events, 0 disables 
{
  unsigned int ticks = stream->parseInt();
  autoInterval = ticks;
  unsigned int currentTick = millis(); // truncate to a 16 bit value
  nextTrigger = currentTick + autoInterval; // set the next trigger tick count
    printf("auto report set to %u for service %c\n",ticks, ServiceId);
}

void asipServiceClass::reportError( const char svc, const char request, asipErr_t errno, Stream *stream)
{
   asip.sendErrorMessage(svc,request, errno, stream);
}