/*
 * asipTone.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2015 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


#ifndef asipTone_h
#define asipTone_h

#include "asip.h"

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

//IDs already used by the core mirtle services:  I,M,E,B,R

// RGB Pixel service
const char id_TONE_SERVICE = 'T';
// methods
const char tag_PLAY = 'P';            // play tone of given frequency and duration

class asipToneClass : public asipServiceClass
{  
public:
   asipToneClass(const char svcId, const char evtId);
   void begin(int speakerPin);
   void reset();
   void reportValues(Stream * stream);
   void reportValue(int sequenceId, Stream * stream) ; // not used in this service  
   void processRequestMsg(Stream *stream);  
private: 
    int speakerPin;
};   

extern asipToneClass asipTone;

#endif
 
   


