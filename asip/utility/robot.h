/*
 * robot.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef robot_h
#define robot_h
#include "asip.h"

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

// Motor service
const char id_MOTOR_SERVICE = 'M';
// Motor methods (messages to Arduino)
const char tag_SET_MOTOR     = 'm';  
const char tag_SET_MOTORS    = 'M';
const char tag_STOP_MOTOR    = 's';  
const char tag_STOP_MOTORS   = 'S';

// Encoder service
const char id_ENCODER_SERVICE = 'E';
// Encoder methods - use system define, tag_AUTOEVENT_REQUEST ('A') to request autoevents
// Encoder events -  events use system tag: tag_SERVICE_EVENT  ('e')


// Bump detect service
const char id_BUMP_SERVICE = 'B';
// Bump sensor methods - use system define, tag_AUTOEVENT_REQUEST ('A') to request autoevents
// Bump Sensor events -  events use system tag: tag_SERVICE_EVENT  ('e')


// IR Line detect service
const char id_IR_REFLECTANCE_SERVICE = 'R';
// IR Line detect methods - use system define, tag_AUTOEVENT_REQUEST ('A') to request autoevents
// IR Line detect events -  events use system tag: tag_SERVICE_EVENT  ('e')


const int NBR_WHEELS = 2;  // defines the number of wheels (and encoders), note not tested with values other than 2

class robotMotorClass : public asipServiceClass
{  
public:

   robotMotorClass(const char svcId, const char evtId);  
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void reset();
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void reportValues(Stream *stream);
   void setMotor(byte motor, int speed);
   void setMotors(int speed0, int speed1);
   void stopMotor(byte motor);
   void stopMotors();
   void processRequestMsg(Stream *stream);
 };
   
class encoderClass : public asipServiceClass
{  
public:
   encoderClass(const char svcId);
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void reset();
   void reportValues(Stream * stream);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device   
   void processRequestMsg(Stream *stream);
private:
  unsigned long pulse[NBR_WHEELS];  
  long count[NBR_WHEELS];  
};   
   

class bumpSensorClass : public asipServiceClass
{  
public:
   bumpSensorClass(const char svcId);
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void reset();
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void processRequestMsg(Stream *stream);
};

class irLineSensorClass : public asipServiceClass
{  
public:
   irLineSensorClass(const char svcId);
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void reset();
   void reportValues(Stream *stream);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void processRequestMsg(Stream *stream);
};    


#endif
 
   


