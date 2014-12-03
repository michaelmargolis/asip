/*
 * asipNeoPixels.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis + Franco Raimondi
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


#ifndef asipNeoPixels_h
#define asipNeoPixels_h

#include "asip.h"
#include <Adafruit_NeoPixel.h>

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

//IDs already used by the core mirtle services:  I,M,E,B,R,S (servo),D (distance)

// NeoPixel service
const char id_NEOPIXELS_SERVICE = 'N';
// methods
const char tag_NEOPIXELS_SETPIXELCOLOR = 'C';
const char tag_NEOPIXELS_SETBRIGHTNESS = 'B';
const char tag_NEOPIXELS_SHOW = 'S';

   
class asipNeoPixelsClass : public asipServiceClass
{  
public:
   asipNeoPixelsClass(const char svcId, const char evtId);
//   void begin(byte nbrPixels, const pinArray_t pins[], Servo* servos);
   void begin(int nbrStrips, const pinArray_t pins[], Adafruit_NeoPixel* strip);
   void reset();
   void reportValues(Stream * stream);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device   
   void processRequestMsg(Stream *stream); 
   //void reportName(Stream *stream);   
private: 
   Adafruit_NeoPixel *myStripPtr;
   void remapPins(Stream *stream);
   void show(int n);
    void setPixelColor(int n, int p, int r, int g, int b);
   void setBrightness(int n, int b);
};

extern asipNeoPixelsClass asipNeoPixels;

#endif