/*
 * asipDistance.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */
 
#include "asipDistance.h"

static PROGMEM const prog_char myName[]  = "Distance";
 
asipDistanceClass::asipDistanceClass(const char svcId) : asipServiceClass(svcId)
{
   svcName = myName;
}

// each sensor uses 1 pin
void asipDistanceClass::begin(byte nbrElements, const pinArray_t pins[])
{
  asipServiceClass::begin(nbrElements,pins);

}

// this function rewrites the pins used by this service   
void asipDistanceClass::remapPins(Stream *stream)
{

   int count = stream->parseInt();
   if( count == pinCount && count <= nbrElements) {
       for(int i=0; i < count; i++){
	   // todo 
       }	      
   }
   else {
     // error message
   }
}

/*
void asipDistanceClass::reportName(Stream * stream)
{
  stream->println(F(ASIP_SERVICE_NAME));
}
*/
 void asipDistanceClass::reset()
 {
  
 }
 
 void asipDistanceClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
  if( sequenceId < nbrElements) {
       stream->print(getDistance(sequenceId));
  }
}

void asipDistanceClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   if( request == tag_AUTOEVENT_REQUEST) {
      setAutoreport(stream);
   }
   else if(request == tag_DISTANCE_MEASURE){ 
      reportValues(stream);  // send a single measurement
   }
      
   else {
     reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}

int asipDistanceClass::getDistance(int sequenceId)
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

 