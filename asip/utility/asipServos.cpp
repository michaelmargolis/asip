/*
 * asipServos.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */
 
#include "asipServos.h"

static PROGMEM const prog_char myName[]  =  "Servos";

asipServoClass::asipServoClass(const char svcId, const char evtId)
  :asipServiceClass(svcId)
{
   svcName = myName;
}

// each servo uses 1 pin
 void asipServoClass::begin(byte nbrElements, const pinArray_t pins[], Servo* servoPtr )
{
  //each servo uses one so pinCount equals nbrElements 
  asipServiceClass::begin(nbrElements,nbrElements,pins);
  myServoPtr = servoPtr; 
  for(int i=0; i < nbrElements; i++) {     
     myServoPtr[i].attach(pins[i]);
     printf("Attaching servo id %d to pin %d\n", i, pins[i]);
   }
}

void asipServoClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
   // this class does not report
}

 void asipServoClass::reportValues(Stream *stream)
{
  // this class does not report
}
 
 /*
void asipServoClass::reportName(Stream * stream)
{
  stream->println(F(ASIP_SERVICE_NAME));
}
 */
 
void asipServoClass::write(byte servoId, byte angle)
{
   if(servoId < nbrElements){
       angle = constrain(angle,0,180);
       myServoPtr[servoId].write(angle);
       printf("Servo id %d on pin %d moving to %d degrees\n", servoId, pins[servoId], angle);
   }
}
   
// this function rewrites the pins used by this service   
void asipServoClass::remapPins(Stream *stream)
{
  pinArray_t newPins[nbrElements]; // pin map request

   int count = stream->parseInt();
   if( count == pinCount && count <= nbrElements) {
       for(int i=0; i < count; i++){
           newPins[i] = stream->parseInt(); 
           // todo - add a check here for pin within range and not reserved
        }         
        // if all pins are valid then detach the old pins and attach the new
        for(int i=0; i < count; i++){
          myServoPtr[i].detach();    
          //////////TODO - FIX THIS !!!!!!!!!!!!!!!
          asip.registerPinMode(pins[i], UNALLOCATED_PIN_MODE, ServiceId); // deregister the pin old
          myServoPtr[i].attach(newPins[i]);  
          asip.registerPinMode(newPins[i], OTHER_SERVICE_MODE, ServiceId); // register the new pin
        }      
   }
   else {
     // error message
   }
}
 
void asipServoClass::reset()
{

}
 
void asipServoClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   int angle;
   if(request == tag_SERVO_WRITE) {
     int servoId = 0;  
     if(nbrElements == 0) {
       reportError(ServiceId, request, ERR_DEVICE_NOT_AVAILABLE, stream);
     }
     else if(nbrElements > 0) {
       // must send servo id even if only one servo
        servoId = stream->parseInt(); // a single servo has an ID of 0;
        if(servoId > nbrElements-1) {
          reportError(ServiceId, request, ERR_INVALID_DEVICE_NUMBER, stream);
        }  
        else {
          int angle = stream->parseInt();         
          write(servoId, angle);         
        }               
     }
   } 
   else if( request == tag_REMAP_PIN_REQUEST) {
       remapPins(stream);
   }
   else {
      reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }   
}
