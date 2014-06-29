/*
 * asipIO.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "asipIO.h"

//#define ASIP_SERVICE_NAME  ((asipSvcName) "ASIP core IO")
//THIS_SVC_NAME("ASIP core IO");


#ifdef PRINTF_DEBUG
// mode strings for debug
char * modeStr[] = {"UNALLOCATED", "INPUT", "INPUT_PULLUP", "OUTPUT", "ANALOG", "PWM","INVALID","OTHER SERVICE", "RESERVED"}; 
#endif

void AssignPort(byte pin); // store register associated with given pin in register table 
byte getPortIndex(byte pin); // return index into portRegisterTable associated with given pin
void reportDigitalPins(Stream *stream, byte pin, boolean report); //set or clear flag indicating reporting of this digital input
void checkPinChange();

PROGMEM const prog_char myName[]  = "ASIP core IO";
//asipSvcName myName[] PROGMEM = "ASIP core IO";

asipIOClass asipIO(id_IO_SERVICE,tag_ANALOG_VALUE ); 




  asipIOClass::asipIOClass(const char svcId, const char evtId )
  :asipServiceClass(svcId,evtId )
{
 svcName = myName;
}

void asipIOClass::begin( )
{
  // arguments are not needed because the hardware is accessed directly through registers
  analogInputsToReport = 0;
  for( byte p = 0; p < NUM_DIGITAL_PINS; p++) {
     AssignPort(p);
   }
}

void asipIOClass::begin(byte nbrElements, byte pinCount, const pinArray_t pins[])
{

}

void asipIOClass::reset()
{
  for( byte p = 0; p < NUM_DIGITAL_PINS; p++) {
    if( asip.getPinMode(p) < RESERVED_MODE) {  
       if( asip.deregisterPinMode(p) == ERR_NO_ERROR){
	    // here if pin not  reserved or owned by other services
		pinMode(p, INPUT); // this is the default Arduino pin state at startu-up
	   }
	   else{
	    printf("Error de-registering pin %d\n", p);
	  }
	 }
   }
}


void asipIOClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
}

 void asipIOClass::reportValues(Stream *stream)
{
  // report analog values 
  if( nbrActiveAnalogPins > 0 )  { 
    stream->write(EVENT_HEADER);
    stream->write(ServiceId);
    stream->write(',');
    stream->write(tag_ANALOG_VALUE);
    stream->write(',');
    stream->print(nbrActiveAnalogPins);
	stream->write(',');  // comma added 21 June 2014
    stream->write('{');
    for( byte pin=0, count=0; pin < MAX_ANALOG_INPUTS; pin++) {     
      if( analogInputsToReport & (1U << pin) ) { 
         stream->print(pin);
         stream->write(':');
         stream->print(analogRead(pin));
         if( ++count != nbrActiveAnalogPins)
           stream->write(',');
         else   
           stream->println("}"); 
      }      
    }
    stream->write(MSG_TERMINATOR); 
  } 
}

void asipIOClass::processRequestMsg(Stream *stream)
{
   char request = stream->read();
   byte pin;
   int value;  
   if( request == tag_PIN_MODE || request == tag_DIGITAL_WRITE || request == tag_ANALOG_WRITE) {
     pin = stream->parseInt();
     value = stream->parseInt();
     VERBOSE_DEBUG( printf("Request %d for pin %d with val=%d\n", request, pin,value);)
   }
   asipErr_t err = ERR_NO_ERROR;   
   switch(request) {
      case tag_ANALOG_DATA_REQUEST:  
          setAutoreport(stream);       
          break;
      case tag_GET_PORT_TO_PIN_MAPPING:
          asip.sendPortMap();
          break;
      case tag_GET_PIN_MODES:
          asip.sendPinModes();
          break;
	  case tag_GET_PIN_CAPABILITIES:
           asip.sendPinCapabilites();
           break;		   
      case tag_PIN_MODE:
          err = PinMode(stream, pin, value);      
          break;
	  case tag_GET_PIN_SERVICES_LIST:
           asip.sendPinServicesList();	  
		   break;
      case tag_DIGITAL_WRITE:
         err = DigitalWrite(pin,value);         
         break; 
      case tag_ANALOG_WRITE:
         err = AnalogWrite(pin,value);      
         break;
      default:
         err = ERR_UNKNOWN_REQUEST;       
   }
   if( err != ERR_NO_ERROR){
       asip.sendErrorMessage(id_IO_SERVICE, request, err, stream);
   }
}

void asipIOClass::reportAnalogPin(byte analogPin, boolean report)  // sets pin mode and flag for unsolicited messages 
                                                                   // the autoInterval must be non-zero for message to be sent
                                                                   // see ANALOG_DATA_REQUEST for the message to set the interval  
{ 
  if (analogPin < MAX_ANALOG_INPUTS) {
    if(report == true) {      
      analogInputsToReport |= (1U << analogPin); 
    } else {
      analogInputsToReport &= ~(1U << analogPin);
    }
    // recalculate the number of active pins.
    nbrActiveAnalogPins=0;
    for( byte pin =0 ; pin < MAX_ANALOG_INPUTS; pin++){
       if(analogInputsToReport & (1U << pin) ) {         
          ++nbrActiveAnalogPins;          
        }
    }  
  }
}

void asipIOClass::reportDigitalPin(byte pin,boolean report)
{

}

/*
 Pin requests are by their digital number, by default, requests will only succeed if
 the pin exists,supports the requested mode and is not reserved or allocated to a service.  
 */
asipErr_t asipIOClass::PinMode(Stream * stream, byte pin, int mode)
{
  //printf("Request pinmode %s (%d) for pin %d\n", mode >=0 ? modeStr[mode] : modeExtStr[mode+3],mode,  pin);
  printf("Request pinmode %s (%d) for pin %d\n", modeStr[mode],mode,  pin);
  asipErr_t err = ERR_INVALID_MODE;
  if (IS_PIN_ANALOG(pin)) {
    reportAnalogPin(PIN_TO_ANALOG(pin), mode == ANALOG_MODE ); // turn on/off reporting
  }
  if (IS_PIN_DIGITAL(pin)) {
    if (mode == INPUT_MODE || mode == INPUT_PULLUP_MODE) {
       reportDigitalPins(stream, pin,true); 
    } else {
       reportDigitalPins(stream, pin,false); 
    }
  }
  switch(mode) {
  case ANALOG_MODE:
    if (IS_PIN_ANALOG(pin)) {
      if (IS_PIN_DIGITAL(pin)) {
        pinMode(PIN_TO_DIGITAL(pin), INPUT);    // disable output driver
        digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable internal pull-ups
      }
     err = asip.registerPinMode(pin,ANALOG_MODE, ServiceId);
    }
    break;
  case  INPUT_PULLUP_MODE:    
    if (IS_PIN_DIGITAL(pin)) {
      pinMode(PIN_TO_DIGITAL(pin), INPUT_PULLUP); // disable output driver      
      err = asip.registerPinMode(pin,INPUT_PULLUP_MODE, ServiceId); 
    }
    break;
  case INPUT_MODE:
    if (IS_PIN_DIGITAL(pin)) {
      pinMode(PIN_TO_DIGITAL(pin), INPUT); // disable output driver
      digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable internal pull-ups
      err = asip.registerPinMode(pin,INPUT_MODE, ServiceId);
    }
    break;
  case OUTPUT_MODE:
    if (IS_PIN_DIGITAL(pin)) {
      digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable PWM
      pinMode(PIN_TO_DIGITAL(pin), OUTPUT_MODE);
      err = asip.registerPinMode(pin,OUTPUT_MODE, ServiceId);
    }
    break;
  case PWM_MODE:
    if (IS_PIN_PWM(pin)) {
      pinMode(PIN_TO_PWM(pin), OUTPUT_MODE);
      analogWrite(PIN_TO_PWM(pin), 0);
      err = asip.registerPinMode(pin,PWM_MODE, ServiceId);
    }
    break;
   case UNALLOCATED_PIN_MODE: //Free up pin and restore to startup state
     if (IS_PIN_DIGITAL(pin)) {
      pinMode(PIN_TO_DIGITAL(pin), INPUT); // disable output driver
      digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable internal pull-ups
      err = asip.registerPinMode(pin,UNALLOCATED_PIN_MODE, ServiceId);
    }
    break;
  }
  return err;
}

asipErr_t asipIOClass::AnalogWrite(byte pin, int value)
{
  printf("AnalogWrite %d on pin %d\n", value, pin);
  asipErr_t err = ERR_NO_ERROR;
  if (pin < NUM_DIGITAL_PINS  && IS_PIN_PWM(pin)){
     if( asip.getPinMode(pin) == PWM_MODE) {     
        analogWrite(PIN_TO_PWM(pin), value);      
     }
     else {
      err = ERR_WRONG_MODE;
     }
  }
  else {
     err = ERR_INVALID_PIN;
  }  
  return err;
}

asipErr_t asipIOClass::DigitalWrite(byte pin, byte value)
{
  printf("DigitalWrite %d on pin %d\n", value, pin);
  asipErr_t err = ERR_NO_ERROR;
  if (pin < NUM_DIGITAL_PINS  ){
     if( asip.getPinMode(pin) == OUTPUT_MODE || asip.getPinMode(pin) == INPUT_MODE || asip.getPinMode(pin) == INPUT_PULLUP_MODE){    // enable setting of pullups
         digitalWrite(pin, value);
     }
     else {
       err = ERR_WRONG_MODE;
     }
  }
  else {
     err = ERR_INVALID_PIN;
  }  
  return err; 
}

// code to support reporting of digital ports
// allow lots of ports if Mega or Arduino DUE   
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  || defined(__SAM3X8E__) 
const int MAX_IO_PORTS = 11;
#else
const int MAX_IO_PORTS = 6;  // this could be reduced to 3 for the Uno and similar
#endif 

const byte PORT_ERROR = 255; // the request for a port cannot be resolved (bad pin or port)


   /* digital input ports */
   static byte reportPinMasks[MAX_IO_PORTS];    // for each bit position, 1=report this port, 0=silence
   static byte previousPINs[MAX_IO_PORTS];      // previous 8 bits sent
   static byte portRegisterTable[MAX_IO_PORTS]; // a list of the actual port on this chip registers
   static byte portCount = 0;                   // the number of actual ports on this chip
 

// store the register associated with the given pin in the register table 
void AssignPort(byte pin)
{
  byte port = digitalPinToPort(pin); 
  byte index = 0;
  while(true) {
     if( portRegisterTable[index] == port)
        return; // already assigned
     if( index < portCount)
          index++;
     else 
        break;                
  }
  portRegisterTable[index] = port;
  portCount++;
  //VERBOSE_DEBUG (printf("Added index %d for port %d\n", index,port);)
}    

// return the index into the portRegisterTable associated with the given pin
byte getPortIndex(byte pin) 
{
   byte port = digitalPinToPort(pin); 
   for( byte index = 0; index < portCount; index++) {
       if(portRegisterTable[index] == port) {
           // printf("Port index for pin %d is %d\n", pin, index);
           return index;
       }
   }
   return PORT_ERROR;
}

void reportDigitalPins(Stream *stream, byte pin, boolean report)
{
// todo - add error checking here
   byte portIndex = getPortIndex(pin);
   byte mask = digitalPinToBitMask(pin);  
   if(report) {
      reportPinMasks[portIndex] |= mask;
	   sendDigitalPortChanges(stream,true);
    }
    else
      reportPinMasks[portIndex] &= (~mask);
    VERBOSE_DEBUG( printf("Port index for pin %d is %d, mask=%xX\n", pin, portIndex, reportPinMasks[portIndex]); )    
}

// this function is repeatedly called by the main asip service routine
void sendDigitalPortChanges(Stream * stream, bool sendIfNotChanged)
{
   for( byte i=0; i < portCount; i++ ) {
      if(reportPinMasks[i] != 0) {
         byte port = portRegisterTable[i];
         byte data = *portInputRegister(port) & reportPinMasks[i];
         if( (data != previousPINs[i]) || sendIfNotChanged ){            
            stream->write(EVENT_HEADER);
            stream->write(id_IO_SERVICE);
            stream->write(',');
            stream->write(tag_PORT_DATA);
            stream->write(',');
            stream->print(port);
            stream->write(',');
            stream->print(data,HEX); 
            stream->write(MSG_TERMINATOR); 			
            previousPINs[i] = data; 
         }  
      }
   }
}
   
 



