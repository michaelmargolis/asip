/*
 * OtherServices.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */
 
#include "OtherServices.h"

//declare servo object(s) 
const byte NBR_SERVOS = 1;
Servo myServos[NBR_SERVOS];  // create one servo object


servoClass servos(SERVO_SERVICE, NO_EVENT);

servoClass::servoClass(const char svcId, const char evtId)
  :asipServiceClass(svcId,evtId)
{}

// each servo uses 1 pin
 void servoClass::begin(byte nbrElements, byte pinCount, const pinArray_t pins[])
{
  asipServiceClass::begin(nbrElements,pinCount,pins);
  if(pinCount <= NBR_SERVOS) { 
    for(int i=0; i < nbrElements; i++) {     
       myServos[i].attach(pins[i]);
	   printf("Attaching servo id %d to pin %d\n", i, pins[i]);
    }
  } 
  else {
     printf("Not enough servos\n");
  }
}

void servoClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
   // this class does not report
}

 void servoClass::reportValues(Stream *stream)
{
  // this class does not report
}
  
void servoClass::write(byte servoId, byte angle)
{
   if(servoId < NBR_SERVOS){
       angle = constrain(angle,0,180);
       myServos[servoId].write(angle);
	   printf("Servo id %d on pin %d moving to %d degrees\n", servoId, pins[servoId], angle);
   }
}
   
// this function rewrites the pins used by this service   
void servoClass::remapPins(Stream *stream)
{
static pinArray_t newPins[NBR_SERVOS]; // pin map request

   int count = stream->parseInt();
   if( count == pinCount && count <= NBR_SERVOS) {
       for(int i=0; i < count; i++){
	       newPins[i] = stream->parseInt();	
		   // todo - add a check here for pin within range and not reserved
	    }		  
		// if all pins are valid then detach the old pins and attach the new
		for(int i=0; i < count; i++){
	      myServos[i].detach();	 
          asip.registerPinMode(pins[i], UNALLOCATED_PIN_MODE); // deregister the pin old
		  myServos[i].attach(newPins[i]);	 
		  asip.registerPinMode(newPins[i], OTHER_SERVICE_MODE); // register the new pin
	    }	   
   }
   else {
     // error message
   }
}
   
void servoClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   int angle;
   if(request == SERVO_WRITE) {
     int servoId = 0;  
	 if(NBR_SERVOS == 0) {
       reportError(ServiceId, request, ERR_DEVICE_NOT_AVAILABLE, stream);
     }
     else if(NBR_SERVOS > 0) {
       // must send servo id even if only one servo
        servoId = stream->parseInt(); // a single servo has an ID of 0;
        if(servoId > NBR_SERVOS-1) {
          reportError(ServiceId, request, ERR_INVALID_DEVICE_NUMBER, stream);
        }  
        else {
          int angle = stream->parseInt();		  
          write(servoId, angle);		 
        }				
     }
   } 
   else if( request == REMAP_PIN_REQUEST) {
       remapPins(stream);
   }
   else {
      reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }   
}

distanceSensorClass distanceSensor(DISTANCE_SERVICE, DISTANCE_EVENT);

distanceSensorClass::distanceSensorClass(const char svcId, const char evtId) : asipServiceClass(svcId,evtId){}

// each sensor uses 1 pin
void distanceSensorClass::begin(byte nbrElements, byte pinCount, const pinArray_t pins[])
{
  asipServiceClass::begin(nbrElements,pinCount,pins);

}

// this function rewrites the pins used by this service   
void distanceSensorClass::remapPins(Stream *stream)
{

   int count = stream->parseInt();
   if( count == pinCount && count <= NBR_SERVOS) {
       for(int i=0; i < count; i++){
	   // todo 
       }	      
   }
   else {
     // error message
   }
}

 void distanceSensorClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
  if( sequenceId < nbrElements) {
       stream->print(getDistance(sequenceId));
  }
}

void distanceSensorClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   if( request == DISTANCE_REQUEST) {
      setAutoreport(stream);
   }
   else if(request == DISTANCE_MEASURE){ 
      reportValues(stream);  // send a single measurement
   }
      
   else {
     reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}

int distanceSensorClass::getDistance(int sequenceId)
{
const long MAX_DISTANCE = 100;  
const long MAX_DURATION =   (MAX_DISTANCE * 58);

  // The sensor is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  byte pin = pins[sequenceId]; 
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(4);
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);

  pinMode(pin, INPUT); 
  
  // limit pulseIn duration to a max of 275cm (just under 16ms) 
  // if pulse does not arrive in this time then ping sensor may not be connected
  // if you need to increase this then you must change the distanceSensorDataRequest message body size
  long duration = pulseIn(pin, HIGH, MAX_DURATION); 
  // convert the time into a distance
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  int cm = (duration / 29) / 2;
  return cm;  
}

 