/*
 * asipDistance.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


#ifndef asipDistance_h
#define asipDistance_h

#include "asip.h"

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

//IDs already used by the core mirtle services:  I,M,E,B,R

// Distance detect service
const char id_DISTANCE_SERVICE = 'D';
// methods
// enable auto events - use system define, AUTOEVENT_REQUEST ('A') to request autoevents
const char  tag_DISTANCE_MEASURE = 'M';   // measure and send a single event 
// events use system tag: SERVICE_EVENT  ('e')


class asipDistanceClass : public asipServiceClass
{  
public:

   asipDistanceClass(const char svcId);  
   void begin(byte nbrElements, const pinArray_t pins[]);
   void reset();
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void processRequestMsg(Stream *stream);
   void remapPins(Stream *stream);
   //void reportName(Stream *stream);
private:
   int getDistance(int sequenceId);
 };
  
extern asipDistanceClass asipDistance;
#endif
 
   


