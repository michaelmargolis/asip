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
const char MOTOR_SERVICE = 'M';
// Motor methods (messages to Arduino)
const char SET_MOTOR     = 'm';  
const char SET_MOTORS    = 'M';
const char STOP_MOTOR    = 's';  
const char STOP_MOTORS   = 'S';

// Encoder service
const char ENCODER_SERVICE = 'E';
// Encoder methods
const char ENCODER_REQUEST = 'R';
// Encoder events
const char ENCODER_EVENT   = 'e';


// Bump detect service
const char BUMP_SERVICE = 'B';
// Encoder methods
const char BUMP_REQUEST = 'R';
// Encoder events
const char BUMP_EVENT  =  'e';

// IR Line detect service
const char IR_REFLECTANCE_SERVICE = 'R';
// Encoder methods
const char  IR_LINE_REQUEST = 'R';
// Encoder events
const char IR_LINE_EVENT   = 'e';


const int NBR_WHEELS = 2;  // defines the number of wheels (and encoders), note not tested with values other than 2

class robotMotorClass : public asipServiceClass
{  
public:

   robotMotorClass(const char svcId, const char evtId);  
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
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
   encoderClass(const char svcId, const char evtId);
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
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
   bumpSensorClass(const char svcId, const char evtId);
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void processRequestMsg(Stream *stream);
};

class irLineSensorClass : public asipServiceClass
{  
public:
   irLineSensorClass(const char svcId, const char evtId);
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void reportValues(Stream *stream);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void processRequestMsg(Stream *stream);
};    


extern robotMotorClass motors;
extern encoderClass encoders;
extern bumpSensorClass bumpSensors;
extern irLineSensorClass irLineSensors;
#endif
 
   


