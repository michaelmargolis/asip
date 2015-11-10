/*
 * asipTone.cpp -  Arduino Services Interface Protocol (ASIP)
 * tone requests are non-blocking, services should delay for the desired
 * interval between sequences of tones.
 * 
 * Copyright (C) 2015 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

 #include <asipTone.h>
 
 asipToneClass::asipToneClass(const char svcId, const char evtId)
  :asipServiceClass(svcId)
{
  svcName = PSTR("Tone");
}

void asipToneClass::begin(int pin)
{
  speakerPin = pin;
}

void asipToneClass::reset()
{
   noTone(speakerPin);
}

void asipToneClass::reportValues(Stream * stream)
{
}

void asipToneClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device   
{
   stream->println("Tone");
}

void asipToneClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();  
   if( request == tag_PLAY) {
      int frequency = stream->parseInt();  
      int duration = stream->parseInt(); 
      if( frequency > 0 && duration > 0){
          tone(speakerPin, frequency, duration);
      }      
   }     
}
 

 
   


