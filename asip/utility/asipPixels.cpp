/*
 * asipPixels.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2015 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

 #include <asipPixels.h> 
 #include <Adafruit_NeoPixel.h>
 #include "robot_pins.h"

#define NUMBER_OF_PIXELS 18  // increase this if the number of pixels is greater  
#ifdef  neoPixelPin
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_OF_PIXELS, neoPixelPin, NEO_GRB + NEO_KHZ800);
 
 asipPixelsClass::asipPixelsClass(const char svcId, const char evtId)
  :asipServiceClass(svcId)
{
  svcName = PSTR("Pixels");
}

void asipPixelsClass::begin()
{
    strip.begin();  // initialize the NeoPixel library.
}

void asipPixelsClass::reset()
{
    strip.begin();
}

void asipPixelsClass::reportValues(Stream * stream)
{
}

void asipPixelsClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device   
{
   stream->println("Pixels");
}

void asipPixelsClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();  
   //stream->print("processing request "); stream->write(request); stream->println();
   if( request == tag_SET_PIXELS) 
   {
     //Comma separated pairs of colon separated pixel positions and hex color values
      // request:  "P,P,count,{pixel:color,...}\n"  // first pixel is 0   
      int count = stream->parseInt();  
      if( stream->read() == ',' && stream->read() == '{') // skip to start of parms
      {           
        // todo - check to ensure  count, data pairs, and closing bracket are consistent
        while(count--)
        {
           int pixel = stream->parseInt();
           unsigned long color = stream->parseInt();
           strip.setPixelColor(pixel, color);
        }
        strip.show();        
      }
    }  
    else if(request == tag_SET_PIXEL_SEQUENCE)
    {
    // Comma separated hex color values for pixels starting from positon given in the First Pixel field
     // request:  "P,S,count,firstPixel,{color,color,...}\n"  
      int count = stream->parseInt();  
      int firstPixel = stream->parseInt();  
      if(stream->read() == ',' && stream->read() == '{') // skip to start of parms
      {           
        // todo - check to ensure  count, data pairs, and closing bracket are consistent
        while(count--)
        {          
           unsigned long color = stream->parseInt();
           strip.setPixelColor(firstPixel++, color);
        }
        strip.show();        
      }    
       
    }
    else if(request == tag_SET_BRIGHTNESS)
    {
        int brightness = stream->parseInt();
        // sets overall strip brightness, 255 is max
        strip.setBrightness(brightness);
        strip.show();  
    }
    else if(request == tag_GET_NUMBER_PIXELS)
    {
       // request:  "P,I\n"  
       // reply:  @P,I,count\n"      
       stream->write(EVENT_HEADER);
       stream->write(id_PIXELS_SERVICE);
       stream->write(',');
       stream->write(tag_GET_NUMBER_PIXELS);
       stream->write(',');
       stream->print(NUMBER_OF_PIXELS); // this is max that can be connected, actual may be less
       stream->write(MSG_TERMINATOR);        
    }     
    else
    {
      reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
    }
    while( stream->available() && stream->read() != '\n') 
         ; // skip to end of line
}
#endif
// private methods



