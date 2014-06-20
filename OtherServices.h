/*
 * OtherServices.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


#ifndef OtherServices_h
#define OtherServices_h

#include "asip.h"

#include <Servo.h> 

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

//IDs already used by the core mirtle services:  I,M,E,B,R

// Distance detect service
const char DISTANCE_SERVICE = 'D';
// methods
const char  DISTANCE_REQUEST = 'R';   // enable auto events
const char  DISTANCE_MEASURE = 'M';   // measure and send a single event 
// events
const char  DISTANCE_EVENT =   'e';

// Servo service
const char SERVO_SERVICE = 'S';
// methods
const char SERVO_WRITE = 'W';


class distanceSensorClass : public asipServiceClass
{  
public:

   distanceSensorClass(const char svcId, const char evtId);  
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void processRequestMsg(Stream *stream);
   void remapPins(Stream *stream);
private:
   int getDistance(int sequenceId);
 };
   
class servoClass : public asipServiceClass
{  
public:
   servoClass(const char svcId, const char evtId);
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void reportValues(Stream * stream);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device   
   void processRequestMsg(Stream *stream);      
private: 
   void write(byte servoId, byte angle);
   void remapPins(Stream *stream);
};   
   
extern distanceSensorClass distanceSensor;
extern servoClass servos;
#endif
 
   


