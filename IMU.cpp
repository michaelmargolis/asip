/*
 * IMU.cpp -  Inertial Measurement Unit for Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */
 
#include "IMU.h"
// I2C Arduino libraries (see https://github.com/jrowberg/i2cdevlib)             
#include <I2Cdev.h>   // abstracts bit and byte I2C R/W functions 
#include <MPU6050.h>  // the gyro/accelerometer chip library
#include <HMC5883L.h> // magnetometer chip library

#if defined USE_IMU
MPU6050 accelgyro;  // the MPU6050 supports accelerometer and gyro
HMC5883L mag;

// Gyro service
gyroClass gyro3Axis(GYRO_SERVICE, GYRO_EVENT); // make an instance for the user

gyroClass::gyroClass(const char svcId, const char evtId) : asipServiceClass(svcId,evtId){}

void gyroClass::begin(byte nbrElements)
{
   nbrElements = constrain(nbrElements, 0, NBR_GYRO_AXIS);     
   asipServiceClass::begin(nbrElements);  
   accelgyro.initialize();   // calling this multiple times at startup is ok

}

 void gyroClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
  if( sequenceId < nbrElements) {
       stream->print(axis[sequenceId]);
  }
}

void gyroClass::reportValues(Stream *stream) // send all values separated by commas, preceded by header and terminated with newline
{
  accelgyro.getRotation( &axis[0], &axis[1], &axis[2]);
  asipServiceClass::reportValues(stream); // the base class reports the data
}

void gyroClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   if( request == GYRO_REQUEST) {
      setAutoreport(stream);
   }
   else if(request == GYRO_MEASURE){ 
      reportValues(stream);  // send a single measurement
   }
      
   else {
     reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}

// Accelerometer service
AccelerometerClass accelerometer3Axis(ACCELEROMETER_SERVICE, ACCELEROMETER_EVENT); // make an instance for the user

AccelerometerClass::AccelerometerClass(const char svcId, const char evtId) : asipServiceClass(svcId,evtId){}

void AccelerometerClass::begin(byte nbrElements)
{
   nbrElements = constrain(nbrElements, 0, NBR_ACCEL_AXIS);     
   asipServiceClass::begin(nbrElements);  
   accelgyro.initialize();   // calling this multiple times at startup is ok

}

 void AccelerometerClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
  if( sequenceId < nbrElements) {
       stream->print(axis[sequenceId]);
  }
}

void AccelerometerClass::reportValues(Stream *stream) // send all values separated by commas, preceded by header and terminated with newline
{
  accelgyro.getAcceleration( &axis[0], &axis[1], &axis[2]);
  asipServiceClass::reportValues(stream); // the base class reports the data
}

void AccelerometerClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   if( request == ACCELEROMETER_REQUEST) {
      setAutoreport(stream);
   }
   else if(request == ACCELEROMETER_MEASURE){ 
      reportValues(stream);  // send a single measurement
   }
      
   else {
     reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}

// Heading (Magnetometer) service

HeadingClass heading3Axis(HEADING_SERVICE, HEADING_EVENT); // make an instance for the user

HeadingClass::HeadingClass(const char svcId, const char evtId) : asipServiceClass(svcId,evtId){}

void HeadingClass::begin(byte nbrElements)
{
   nbrElements = constrain(nbrElements, 0, NBR_MAG_AXIS);    
   asipServiceClass::begin(nbrElements);  
   mag.initialize();

}

 void HeadingClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
  if( sequenceId < nbrElements) {
       stream->print(axis[sequenceId]);
  }
}

void HeadingClass::reportValues(Stream *stream) // send all values separated by commas, preceded by header and terminated with newline
{
  mag.getHeading(&axis[0], &axis[1], &axis[2]);
  
  float heading = atan2(axis[1], axis[0]);  // 0 is x axis, 1 is y
  if(heading < 0){
      heading += 2 * M_PI;
  }
  axis[3] =  heading * 180/M_PI	;  
  
  asipServiceClass::reportValues(stream); // the base class reports the data
}

void HeadingClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   if( request == HEADING_REQUEST) {
      setAutoreport(stream);
   }
   else if(request == HEADING_MEASURE){ 
      reportValues(stream);  // send a single measurement
   }
      
   else {
     reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}
#endif



 