/*
 * asipRGBLeds.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2015 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


#ifndef asipRGBLed_h
#define asipRGBLed_h

#include "asip.h"
// include pixel library code here 

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

//IDs already used by the core mirtle services:  I,M,E,B,R

// RGB Pixel service
const char id_RGBLED_SERVICE = 'L';
// methods
const char tag_LED_SET_PIXEL = 'P';   // set pixels using colon separated list of pairs of pixel numbers and hex colors
const char tag_LED_SET_PIXEL = 'S';   // set pixels in sequence using list of colors
const char tag_LED_GET_INFO  = 'I';   // requests message info with number of LEDs (also x,y count if arranged in matrix)

class asipRGBLedClass : public asipServiceClass
{  
public:
   asipRGBLedClass(const char svcId, const char evtId);
   void begin(byte nbrElements, const pinArray_t pins[]);
   void reset();
   void reportValues(Stream * stream);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device   
   void processRequestMsg(Stream *stream); 
   //void reportName(Stream *stream);   
private: 
   void write(byte servoId, byte angle);  
   void remapPins(Stream *stream);
};   

extern asipRGBLedClass asipRGBLed;

#endif
 
   


