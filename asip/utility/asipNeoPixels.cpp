/*
 * asipNeoPixels.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis + Franco Raimondi
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */
 
#include "asipNeoPixels.h"

asipNeoPixelsClass::asipNeoPixelsClass(const char svcId, const char evtId)
  :asipServiceClass(svcId)
{
   svcName = PSTR("NeoPixels");
}

// each strip uses 1 pin;
 void asipNeoPixelsClass::begin(int nbrStrips, const pinArray_t pins[], Adafruit_NeoPixel* stripPtr )
{
  //each strip uses one so pinCount equals nbrElements
  asipServiceClass::begin(sizeof(pins)/sizeof(pinArray_t),
                          sizeof(pins)/sizeof(pinArray_t),
                          pins);
  myStripPtr = stripPtr;
    
//  for(int i=0; i < sizeof(pins)/sizeof(pinArray_t*); i++) {
  for(int i=0; i < sizeof(pins)/sizeof(pinArray_t); i++) {
      myStripPtr[i].begin();
      myStripPtr[i].show();
      // Serial.write("DEBUG: strip ");
      // Serial.print(i, DEC );
      // Serial.write(" initialized \n");
   }
}

void asipNeoPixelsClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
   // this class does not report
}

 void asipNeoPixelsClass::reportValues(Stream *stream)
{
  // this class does not report
}

void asipNeoPixelsClass::reset()
{

}
 
void asipNeoPixelsClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
    
    // Should be N,B,stripId
   if(request == tag_NEOPIXELS_SETBRIGHTNESS) {
     int stripId = 0;
     if(nbrElements == 0) {
       reportError(ServiceId, request, ERR_DEVICE_NOT_AVAILABLE, stream);
     }
     else if(nbrElements > 0) {
       // must send strip id even if only one strip
        stripId = stream->parseInt(); // a single strip has an ID of 0;
        if(stripId > nbrElements-1) {
          reportError(ServiceId, request, ERR_INVALID_DEVICE_NUMBER, stream);
        }  
        else {
          int brightness = stream->parseInt();
          setBrightness(stripId, brightness);
        }               
     }
   }
    
    // Should be N,C,stripId,pin,red,green,blue
   else if( request == tag_NEOPIXELS_SETPIXELCOLOR) {
//       Serial.write("DEBUG: Setting color\n");
       int stripId = 0;
       if(nbrElements == 0) {
           reportError(ServiceId, request, ERR_DEVICE_NOT_AVAILABLE, stream);
       }
       else if(nbrElements > 0) {
           // must send strip id even if only one strip
           stripId = stream->parseInt(); // a single strip has an ID of 0;
           if(stripId > nbrElements-1) {
               reportError(ServiceId, request, ERR_INVALID_DEVICE_NUMBER, stream);
           }
           else {
               int pin = stream->parseInt();
               int red = stream->parseInt();
               int green = stream->parseInt();
               int blue = stream->parseInt();
               setPixelColor(stripId, pin, red, green, blue);
//               Serial.write("DEBUG: done setting color\n");
           }               
       }
   }
    
    // Should be N,S,stripId
   else if( request == tag_NEOPIXELS_SHOW) {
//       Serial.write("DEBUG: Setting show\n");

       int stripId = 0;
       if(nbrElements == 0) {
           reportError(ServiceId, request, ERR_DEVICE_NOT_AVAILABLE, stream);
       }
       else if(nbrElements > 0) {
           // must send strip id even if only one strip
           stripId = stream->parseInt(); // a single strip has an ID of 0;
           if(stripId > nbrElements-1) {
               reportError(ServiceId, request, ERR_INVALID_DEVICE_NUMBER, stream);
           }
           else {
               show(stripId);
//               Serial.write("DEBUG: done setting show\n");
           }
       }
   }
    
   else {
      reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }   
}



// Private methods

void asipNeoPixelsClass::setBrightness(int stripId, int b)
{
    if(stripId < nbrElements){
        int brightness = constrain(b,0,255);
        myStripPtr[stripId].setBrightness((uint8_t) brightness);
    }
}

void asipNeoPixelsClass::setPixelColor(int stripId, int p, int r, int g, int b)
{
    if(stripId < nbrElements){
        int red = constrain(r,0,255);
        int green = constrain(g,0,255);
        int blue = constrain(b,0,255);
        myStripPtr[stripId].setPixelColor((uint16_t) p,
                                          (uint8_t) red,
                                          (uint8_t) green,
                                          (uint8_t) blue);
    }
}

void asipNeoPixelsClass::show(int stripId)
{
    if(stripId < nbrElements){
        myStripPtr[stripId].show();
    }
}