/*
 * asipLCD.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2015 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


#ifndef asipLCD_h
#define asipLCD_h

#include "asip.h"

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

//IDs already used by the core mirtle services:  I,M,E,B,R

// RGB Pixel service
const char id_LCD_SERVICE = 'L';
// methods
const char tag_WRITE = 'W';            // write a line of text
const char tag_CLEAR = 'C';            // clear screen

class asipLCDClass : public asipServiceClass
{  
public:
   asipLCDClass(const char svcId, const char evtId);
   void begin();
   void reset();
   void reportValues(Stream * stream);
   void reportValue(int sequenceId, Stream * stream) ; // not used in this service  
   void processRequestMsg(Stream *stream);  
private: 
  void text(const char *txt, int row);
  void text(const char *txt, int row, int column);
  void show();
  void clear();
  void hGraph(int line, int value);
  void hGraph(char * title, int value1, int value2,int value3,int value4);

};   

extern asipLCDClass asipLCD;

#endif
 
   


