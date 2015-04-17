/*
 * IMU.cpp -  Inertial Measurement Unit for Arduino Services Interface Protocol (ASIP)
 * 
 * These services use I2C Arduino libraries from: https://github.com/jrowberg/i2cdevlib) 
 *
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef ASIP_IMU_h
#define ASIP_IMU_h
#include "asip.h"

#include <Wire.h>

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

//IDs already used by the core mirtle services:  I(I/O),M(Motor),E(Encoder),B(Bump sensor),R(IR line sensors)
//IDs used by other services:  D (distance),S (servo)

// Gyro service
const char id_GYRO_SERVICE = 'G';
// methods
const char  GYRO_REQUEST = 'R';   // enable auto events
const char  GYRO_MEASURE = 'M';   // measure and send a single event 
// events


// Accelerometer service
const char id_ACCELEROMETER_SERVICE = 'A';
// methods
const char  tag_ACCELEROMETER_REQUEST = 'R';   // enable auto events
const char  tag_ACCELEROMETER_MEASURE = 'M';   // measure and send a single event 
// events


// Heading (Magnetometer) service 
const char id_HEADING_SERVICE = 'H';
// methods
const char  tag_HEADING_REQUEST = 'R';   // enable auto events
const char  tag_HEADING_MEASURE = 'M';   // measure and send a single event 
// events


// Pressure (Altitude) service 
const char id_PRESSURE_SERVICE = 'P';
// methods
const char  tag_PRESSURE_REQUEST = 'R';   // enable auto events
const char  tag_PRESSURE_MEASURE = 'M';   // measure and send a single event 
// events


const int NBR_GYRO_AXIS  = 3;
const int NBR_ACCEL_AXIS = 3;
const int NBR_MAG_AXIS   = 4; // 4th element is the calculated Heading direction
const int NBR_PRESSURE_FIELDS = 3; //pressure in mb, temperature C, altitude in meters

class gyroClass : public asipServiceClass
{  
public:

   gyroClass(const char svcId);  
   void begin(byte nbrElements,serviceBeginCallback_t serviceBeginCallback);   // classes that use I2C instead of specific pins use this begin method
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void reportValues(Stream *stream);
   void processRequestMsg(Stream *stream);
   void reset();
private:  
   int axis[NBR_GYRO_AXIS]; // buffer holding readings
 };   

class AccelerometerClass : public asipServiceClass
{  
public:

   AccelerometerClass(const char svcId);  
   void begin(byte nbrElements,serviceBeginCallback_t serviceBeginCallback);   // classes that use I2C instead of specific pins use this begin method
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void reportValues(Stream *stream); // send all values separated by commas, preceded by header and terminated with newline
   void processRequestMsg(Stream *stream);
   void reset();
private:
   int readAxis(int sequenceId);
   int axis[NBR_ACCEL_AXIS];
};   

class HeadingClass : public asipServiceClass
{  
public:

   HeadingClass(const char svcId);  
   void begin(byte nbrElements,serviceBeginCallback_t serviceBeginCallback);   // classes that use I2C instead of specific pins use this begin method
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void reportValues(Stream *stream); // send all values separated by commas, preceded by header and terminated with newline  
   void processRequestMsg(Stream *stream);
   void reset();
private:
   int readAxis(int sequenceId);
   int axis[NBR_MAG_AXIS]; // the 4th element is the calculated Compass direction
};  

// the following is not yet implemented
class PressureClass : public asipServiceClass
{  
public:

   PressureClass(const char svcId);  
   void begin(byte nbrElements,serviceBeginCallback_t serviceBeginCallback);   // classes that use I2C instead of specific pins use this begin method
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void reportValues(Stream *stream); // send all values separated by commas, preceded by header and terminated with newline  
   void processRequestMsg(Stream *stream);
   void reset();
private:
   float field[NBR_PRESSURE_FIELDS];
   int32_t lastMicros; // time of the previous readings
};

extern gyroClass gyro3Axis; 
extern AccelerometerClass accelerometer3Axis;
extern HeadingClass heading3Axis;
extern PressureClass pressure;

#endif

   


