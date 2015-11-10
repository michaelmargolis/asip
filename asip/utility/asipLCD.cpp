/*
 * asipLCD.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2015 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

 #include <asipLCD.h> 

#if defined(__MK20DX256__) // Teensy 3.1 
#include <i2c_t3.h>  // teensy i2c routines
#endif
#include <u8g_i2c.h> // for lcd
   
  static u8g_t u8g;
  static const byte nbrTextLines = 5;
  static const byte charsPerLine = 21;
  static char buffer[nbrTextLines][charsPerLine+1];
 
 asipLCDClass::asipLCDClass(const char svcId, const char evtId)
  :asipServiceClass(svcId)
{
  svcName = PSTR("LCD");
}

void asipLCDClass::begin()
{
    u8g_InitComFn(&u8g, &u8g_dev_sh1106_128x64_i2c, u8g_com_hw_i2c_fn);
    // font , color 
    u8g_SetFont(&u8g, u8g_font_6x12);
    u8g_SetColorIndex(&u8g, 1);    
    clear();
}

void asipLCDClass::reset()
{

}

void asipLCDClass::reportValues(Stream * stream)
{
}

void asipLCDClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device   
{
   stream->println("LCD");
}

void asipLCDClass::processRequestMsg(Stream *stream)
{
  // request:  "L,W,line,string\n"  // top line is line 0
   int request = stream->read();  
   if( request == tag_WRITE) {
      int line = stream->parseInt();  
      if(line < nbrTextLines) {
         if(stream->read() == ',') // skip past comma to get to text
         {
           stream->readBytesUntil('\n', buffer[line], charsPerLine);
           show();
         }
      }
    }  
    else if(request == tag_CLEAR)
    {
       clear();
    }    
}

// private methods
void asipLCDClass::text(const char *txt, int row)
{
   text(txt,row, 0);
}

void asipLCDClass::text(const char *txt, int row, int column)
{
    strlcpy(&buffer[row][column], txt, charsPerLine-column);
    show();
}

void asipLCDClass::show()
{
    u8g_FirstPage(&u8g);
    do {
        for(int y=0; y < nbrTextLines; y++) {
           u8g_DrawStr(&u8g, 0, (y+1)*12, (const char*)&buffer[y][0]);         
        }   
    } while(u8g_NextPage(&u8g)); 
}

void asipLCDClass::clear()
{
      memset(buffer, 0, sizeof(buffer));
      u8g_FirstPage(&u8g);
      do {
         // empty
      } while( u8g_NextPage(&u8g) );
}


void asipLCDClass::hGraph(int line, int value)
{
   // todo
}

void asipLCDClass::hGraph(char * title, int value1, int value2,int value3,int value4)
{
      int width =  u8g_GetWidth(&u8g)-1;
      u8g_FirstPage(&u8g);
      do {
            u8g_DrawStr(&u8g, 0,12, title);
            u8g_DrawBox(&u8g,0,16, map(value1,0,100,0,width),8);
            u8g_DrawBox(&u8g,0,28, map(value2,0,100,0,width),8);
            u8g_DrawBox(&u8g,0,40, map(value3,0,100,0,width),8);
            u8g_DrawBox(&u8g,0,52, map(value4,0,100,0,width),8);
      } while( u8g_NextPage(&u8g) );
    
}

 
   


