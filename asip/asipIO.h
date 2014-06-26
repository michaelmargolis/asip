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

const unsigned int DEFAULT_ANALOG_EVENT_INTERVAL = 20; // not yet used
const byte MAX_ANALOG_INPUTS = min(NUM_ANALOG_INPUTS, sizeof(unsigned int) *8); // the size of the port mask variable
   
//Core IO service
const char id_IO_SERVICE    = 'I';   // tag indicating message is for the low level I/O layer
// IO Methods (messages to Arduino)
const char tag_PIN_MODE      = 'P';   // i/o request  to Arduino to set pin mode
const char tag_DIGITAL_WRITE = 'D';   // i/o request  to Arduino is digitalWrite
const char tag_ANALOG_WRITE  = 'A';   // i/o request to Arduino is analogWrite)
// info requests to Arduino
const char tag_ANALOG_DATA_REQUEST     = 'R'; // request analog data events
const char tag_GET_PORT_TO_PIN_MAPPING = 'M'; // gets a list of pins associated with ports 
const char tag_GET_PIN_MODES           = 'p'; // gets a list of pin modes
const char tag_GET_PIN_SERVICES_LIST   = 's'; // gets a list of pins indicating registered service 
const char tag_GET_PIN_CAPABILITIES    = 'c'; // gets a bitfield array indicating pin capabilities
// IO events (messages from Arduino)
const char tag_PIN_MODES               = 'p'; // the event with a list of pin modes 
const char tag_PORT_DATA               = 'd'; // i/o event with data for a given digital port (tag changed from 'p' 24 June)
const char tag_ANALOG_VALUE            = 'a'; // i/o event from Arduinois value of an analog pin
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
   void reportAnalogPin(byte pin,boolean report);  // sets pin mode and flag for unsolicited messages
   void reportDigitalPin(byte pin,boolean report); // sets pin mode and flag for unsolicited messages
   asipErr_t PinMode(Stream *stream, byte pin, int mode);
   asipErr_t AnalogWrite(byte pin, int value);  
   asipErr_t DigitalWrite(byte pin, byte value); 

    /* analog inputs */
    unsigned int analogInputsToReport; // bitwise array to store pin reporting
    byte nbrActiveAnalogPins;          // the number of pins to report;   
};  


extern asipIOClass asipIO;


#endif