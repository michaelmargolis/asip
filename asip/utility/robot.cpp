/*
 * robot.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "robot.h"
#include "HUBeeWheel.h"

asipSvcName motorName[]   = "Motors";
asipSvcName encoderName[] = "Encoders";
asipSvcName bumpName[]    = "Bump Sensors";
asipSvcName irName[]      = "Reflectance Sensors";


//declare two wheel objects - each encapsulates all the control functions for a wheel
HUBeeBMDWheel wheel[NBR_WHEELS];

robotMotorClass::robotMotorClass(const char svcId, const char evtId)
  :asipServiceClass(svcId)
{
  svcName = motorName;
}

// each motor uses 3 pins, the array order is: m0In1, m0In2, m0pwm, m1In1, m1In2, m1pwm, ...)
// pinCount is three times the number of motors
 void robotMotorClass::begin(byte nbrElements, byte pinCount, const pinArray_t pins[])
{
  asipServiceClass::begin(nbrElements,pinCount,pins);
  wheel[0].setupPins(pins[0], pins[1],pins[2]); //first two pins are for direction control, and 3rd for PWM speed control
  wheel[1].setupPins(pins[3], pins[4],pins[5]);
  for(int i=0; i < NBR_WHEELS; i++) {
     wheel[i].setDirectionMode(0); //Direction Mode determines how the wheel responds to positive and negative motor power values 
     wheel[i].setBrakeMode(0); //Sets the brake mode to zero - freewheeling mode - so wheels are easy to turn by hand
  }
}

void robotMotorClass::reset()
{
  stopMotors();
}

void robotMotorClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
   stream->println("motor");
}

 void robotMotorClass::reportValues(Stream *stream)
{

}
  
void robotMotorClass::setMotor(byte motor, int speed)
{
   if(motor < NBR_WHEELS){
       speed = constrain(speed,-255,255);
       wheel[motor].setMotorPower(speed);
       printf("Motor %d set to %d\n", motor, speed);
   }
}

void robotMotorClass::setMotors(int speed0, int speed1)
{
  // TODO this assumes only two motors
   setMotor(0, speed0);
   setMotor(1, speed1);
}

void robotMotorClass::stopMotor(byte motor)
{
   if(motor < NBR_WHEELS){
       wheel[motor].setMotorPower(0);
       printf("Motor %d stopped\n");
   }
}

void robotMotorClass::stopMotors()
{
    for(int i=0; i < NBR_WHEELS; i++ ){
        wheel[i].setMotorPower(0);
    }
    // todo set flag to reset encoder count on next power up ???
}
   
void robotMotorClass::processRequestMsg(Stream *stream)
{
   int arg0, arg1; 
   int request = stream->read();
   // parse the correct number of arguments for each method 
   if( request == tag_SET_MOTOR || request == tag_SET_MOTORS || request == tag_STOP_MOTOR) {
      arg0 = stream->parseInt();  
      if( request != tag_STOP_MOTOR) {
         arg1 = stream->parseInt();  
      }
   }   
   switch(request) {
      case tag_SET_MOTOR:   setMotor(arg0,arg1);  break;
      case tag_SET_MOTORS:  setMotors(arg0,arg1);break; // TODO this assumes only two motors
      case tag_STOP_MOTOR:  stopMotor(arg0); break;
      case tag_STOP_MOTORS: stopMotors(); break; 
      default: reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}

encoderClass::encoderClass(const char svcId) : asipServiceClass(svcId)
{
  svcName = encoderName; 
}

// each encoder uses 2 pins
void encoderClass::begin(byte nbrElements, byte pinCount, const pinArray_t pins[])
{
  asipServiceClass::begin(nbrElements,pinCount,pins);
  encodersBegin(); // todo - use the pins array instead of hard coding in hubeeWheel.cpp
}

void encoderClass::reportValues(Stream *stream) 
{
  encodersGetData(pulse[0], count[0], pulse[1], count[1]);
  asipServiceClass::reportValues(stream);    
  // pulse width is in microseconds
}

void encoderClass::reset()
{
  encodersReset();
}

 void encoderClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
   if( sequenceId < nbrElements) {
       stream->print(pulse[sequenceId]);
       stream->write(':');   
       stream->print(count[sequenceId]);
    }
}

void encoderClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   if(request == tag_AUTOEVENT_REQUEST) {
     setAutoreport(stream);
   }
   else {
     reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}

bumpSensorClass::bumpSensorClass(const char svcId) : asipServiceClass(svcId)
{
  svcName = bumpName; "Bump Sensors";
}

void bumpSensorClass::bumpSensorClass::begin(byte nbrElements, byte pinCount, const pinArray_t pins[])
{ 
  asipServiceClass::begin(nbrElements,pinCount,pins);
  for(int sw=0; sw < nbrElements; sw++) {     
     pinMode(pins[sw], INPUT_PULLUP); 
  }
}

void bumpSensorClass::reset()
{

}

 void bumpSensorClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
   if( sequenceId < pinCount) {
       //pinMode(pins[sequenceId], INPUT_PULLUP); 
       boolean value = digitalRead(pins[sequenceId]);
       stream->print(value ? "1":"0");
    }
}

void bumpSensorClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   if(request == tag_AUTOEVENT_REQUEST) {
      setAutoreport(stream);
   }
   else {
     reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}

irLineSensorClass::irLineSensorClass(const char svcId) : asipServiceClass(svcId)
{
   svcName = irName;
}

void irLineSensorClass::begin(byte nbrElements, byte pinCount, const pinArray_t pins[]) 
{
  asipServiceClass::begin(nbrElements,pinCount,pins);
  pinMode(pins[0], OUTPUT);
  digitalWrite(pins[0], LOW);
}

void irLineSensorClass::reportValues(Stream *stream) 
{
   // turn on IR emitters
   digitalWrite(pins[0], HIGH);
   delay(5);
   asipServiceClass::reportValues(stream);
    // turn off IR emitters
   digitalWrite(pins[0], LOW);
}

void irLineSensorClass::reset()
{

}

void irLineSensorClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
   if( sequenceId < nbrElements) {
      int pin = pins[sequenceId+1]; // the first pin is the control pin
      pin = PIN_TO_ANALOG(pin); // convert digital number to analog
      int value = analogRead(pin);   
      stream->print(value);
    }
}

void irLineSensorClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   if(request == tag_AUTOEVENT_REQUEST) {
      setAutoreport(stream);
   }
   else {
     reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}



 