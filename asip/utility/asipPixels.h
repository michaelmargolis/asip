/*
 * asipPixels.h -  Arduino Services Interface Protocol (ASIP)
 * Service to set one or more RGB  LEDs, such as WS2812 (neopixels)
 * LEDs are identified by a sequence position number, the first pixel is position 0. 
 * Colors are the RGB values expressed as hex digits.
 * 
 * Copyright (C) 2015 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


#ifndef asipPixels_h
#define asipPixels_h

#include "asip.h"

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

//IDs already used by the core mirtle services:  I,M,E,B,R

// RGB Pixel service
const char id_PIXELS_SERVICE      = 'P';
// methods
const char tag_SET_PIXELS         = 'P';
const char tag_SET_PIXEL_SEQUENCE = 'S';
const char tag_SET_BRIGHTNESS     = 'B';
const char tag_GET_NUMBER_PIXELS  = 'I';

// Colors are the RGB  8 bit values packed into a 32 bit integer as follows:
//      Color = r <<16  +  g <<8  +  b


class asipPixelsClass : public asipServiceClass
{  
public:
   asipPixelsClass(const char svcId, const char evtId);
   void begin();
   void reset();
   void reportValues(Stream * stream);
   void reportValue(int sequenceId, Stream * stream) ; // not used in this service  
   void processRequestMsg(Stream *stream);  
private: 

};   

extern asipPixelsClass asipPixels;

#endif
 
   


