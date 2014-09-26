/*
 * asipIO.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */
 
#ifndef asipIO_h
#define asipIO_h

#include <Arduino.h>
#include "asip.h"

const byte MAX_ANALOG_INPUTS = min(NUM_ANALOG_INPUTS, sizeof(unsigned int) *8); // the size of the port mask variable
 
//Core IO service

const char id_IO_SERVICE    = 'I';   // tag indicating message is for the low level I/O layer
// IO Methods (messages to Arduino)
const char tag_PIN_MODE      = 'P';   // i/o request  to Arduino to set pin mode
const char tag_DIGITAL_WRITE = 'd';   // i/o request  to Arduino is digitalWrite - changed from 'D' to 'd'
const char tag_ANALOG_WRITE  = 'a';   // i/o request to Arduino is analogWrite)    changed from 'A' to 'a' 
// info requests to Arduino
// note that requests for analog data events are with the  system tag_AUTOEVENT_REQUEST tag (changed 1 July)
const char tag_GET_PORT_TO_PIN_MAPPING = 'M'; // gets a list of pins associated with ports 
const char tag_GET_ANALOG_PIN_MAPPING  = 'm'; // gets a list of digital:analog pin associations 
const char tag_GET_PIN_MODES           = 'p'; // gets a list of pin modes
const char tag_GET_PIN_SERVICES_LIST   = 's'; // gets a list of pins indicating registered service 
//const char tag_GET_SERVICES_NAMES    = 'n'; // gets a list of service tags/name pairs 
const char tag_GET_PIN_CAPABILITIES    = 'c'; // gets a bitfield array indicating pin capabilities
// IO events (messages from Arduino)
const char tag_PIN_MODES               = 'p'; // the event with a list of pin modes 
const char tag_PORT_DATA               = 'd'; // i/o event with data for a given digital port (tag changed from 'p' 24 June)
const char tag_ANALOG_VALUE            = 'a'; // i/o event from Arduino is value of an analog pin
const char tag_PIN_CAPABILITIES        = 'c'; // event providing a bitfield array indicating pin capabilities
const char tag_PIN_SERVICES_LIST       = 's'; // event providing a list of pins indicating registered service 

void sendDigitalPortChanges(Stream * stream, bool sendIfNotChanged); // function to send changed digital port data

// this class derives from the service class but has direct access to all pins
class asipIOClass : public asipServiceClass
{  
public:
   asipIOClass(const char svcId, const char evtId);
   void begin(void); // this class does not use the base class begin arguments
   virtual void reset();
   void reportValues(Stream *stream);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device   
   void processRequestMsg(Stream *stream);
private:
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void setAnalogPinAutoReport(byte pin,boolean report);  // sets pin mode and flag for unsolicited messages
   void setDigitalPinAutoReport(byte pin,boolean report); // sets pin mode and flag for unsolicited messages
   asipErr_t PinMode(Stream *stream, byte pin, int mode);
   asipErr_t AnalogWrite(byte pin, int value);  
   asipErr_t DigitalWrite(byte pin, byte value); 

    /* analog inputs */
    unsigned int analogInputsToReport; // bitwise array to store pin reporting
    byte nbrActiveAnalogPins;          // the number of pins to report;   
};  


extern asipIOClass asipIO;


#endif