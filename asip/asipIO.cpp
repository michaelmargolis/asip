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


#ifdef PRINTF_DEBUG
// mode strings for debug
char * modeStr[] = {"UNALLOCATED", "INPUT", "INPUT_PULLUP", "OUTPUT", "ANALOG", "PWM","INVALID","OTHER SERVICE", "RESERVED"}; 
#endif

void AssignPort(byte pin); // store register associated with given pin in register table 
byte getPortIndex(byte pin); // return index into portRegisterTable associated with given pin
void checkPinChange();

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
  byte port = DIGITAL_PIN_TO_PORT(pin); 
  byte index = 0;
  while(true) {
     if( portRegisterTable[index] == port) {
        //VERBOSE_DEBUG (printf("Assign: pin %d has port index %d (port = %d)\n", pin, index,port));
        return; // already assigned
     }
     if( index < portCount) {
          index++;
     }
     else 
        break;                
  }
  portRegisterTable[portCount] = port;
  portCount++;
  //VERBOSE_DEBUG (printf("Assign: added index %d for port %d for pin %d\n", index,port, pin));
}    

// return the index into the portRegisterTable associated with the given pin
byte getPortIndex(byte pin) 
{
  byte port = DIGITAL_PIN_TO_PORT(pin);  
   for( byte index = 0; index < portCount;index++ ) {
       if(portRegisterTable[index] == port) {
           VERBOSE_DEBUG (printf("GetPortIndex: Port index for pin %d is %d\n", pin, index));
           return index;
       }
       printf("GetPortIndex: looking for %d, index= %d, port = %d\n", port, index, portRegisterTable[index]);     
   }
   printf("GetPortIndex: Unable to get index for pin %d on port %d\n", pin, port);
   return PORT_ERROR;
}

/*==============================================================================
 * readPort() - Read an 8 bit port for teensy 3 etc
 *============================================================================*/

static inline unsigned char readPort(byte, byte) __attribute__((always_inline, unused));
static inline unsigned char readPort(byte port, byte bitmask)
{
#if defined(ARDUINO_PINOUT_OPTIMIZE)
        return *portInputRegister(port) & bitmask;
/*      
        if (port == 0) return (PIND & 0xFC) & bitmask; // ignore Rx/Tx 0/1
        if (port == 1) return ((PINB & 0x3F) | ((PINC & 0x03) << 6)) & bitmask;
        if (port == 2) return ((PINC & 0x3C) >> 2) & bitmask;
        return 0;
*/      
#else
        unsigned char out=0, pin=port*8;
        if (IS_PIN_DIGITAL(pin+0) && (bitmask & 0x01) && digitalRead(PIN_TO_DIGITAL(pin+0))) out |= 0x01;
        if (IS_PIN_DIGITAL(pin+1) && (bitmask & 0x02) && digitalRead(PIN_TO_DIGITAL(pin+1))) out |= 0x02;
        if (IS_PIN_DIGITAL(pin+2) && (bitmask & 0x04) && digitalRead(PIN_TO_DIGITAL(pin+2))) out |= 0x04;
        if (IS_PIN_DIGITAL(pin+3) && (bitmask & 0x08) && digitalRead(PIN_TO_DIGITAL(pin+3))) out |= 0x08;
        if (IS_PIN_DIGITAL(pin+4) && (bitmask & 0x10) && digitalRead(PIN_TO_DIGITAL(pin+4))) out |= 0x10;
        if (IS_PIN_DIGITAL(pin+5) && (bitmask & 0x20) && digitalRead(PIN_TO_DIGITAL(pin+5))) out |= 0x20;
        if (IS_PIN_DIGITAL(pin+6) && (bitmask & 0x40) && digitalRead(PIN_TO_DIGITAL(pin+6))) out |= 0x40;
        if (IS_PIN_DIGITAL(pin+7) && (bitmask & 0x80) && digitalRead(PIN_TO_DIGITAL(pin+7))) out |= 0x80;
        return out;
#endif
}
// this function is repeatedly called by the main asip service routine
void sendDigitalPortChanges(Stream * stream, bool sendIfNotChanged)
{
   for( byte i=0; i < portCount; i++ ) {
      if(reportPinMasks[i] != 0) {
         byte port = portRegisterTable[i];
         //byte data = *portInputRegister(port) & reportPinMasks[i];
         byte data =  readPort(port, reportPinMasks[i]);
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

asipIOClass asipIO(id_IO_SERVICE,tag_ANALOG_VALUE ); 




  asipIOClass::asipIOClass(const char svcId, const char evtId )
  :asipServiceClass(svcId,evtId )
{
 svcName = PSTR("ASIP core IO");
}

void asipIOClass::begin( )
{
  // arguments are not needed because the hardware is accessed directly through registers
  analogInputsToReport = 0;
  memset(portRegisterTable, 0xff,MAX_IO_PORTS); // init table to impossible port values prior to assignment 
  for( byte p = 0; p < NUM_DIGITAL_PINS; p++) {
     AssignPort(p);
   }
}

void asipIOClass::begin(byte nbrElements, byte pinCount, const pinArray_t pins[])
{

}

void asipIOClass::reset()
{
  for( byte p = 0; p < TOTAL_PINCOUNT; p++) {
    if( asip.getPinMode(p) < RESERVED_MODE) {  
       if( asip.deregisterPinMode(p) == ERR_NO_ERROR){
        // here if pin not  reserved or owned by other services
        pinMode(p, INPUT); // this is the default Arduino pin state at start-up
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
     //VERBOSE_DEBUG( printf("Request %c for pin %d with val=%d\n", request, pin,value));
   }
   asipErr_t err = ERR_NO_ERROR;   
   switch(request) {
      case tag_AUTOEVENT_REQUEST:       setAutoreport(stream);             break;
      case tag_GET_PORT_TO_PIN_MAPPING: asip.sendPortMap();                break;
      case tag_GET_PIN_MODES:           asip.sendPinModes();               break;
      case tag_GET_PIN_CAPABILITIES:    asip.sendPinCapabilites();         break;          
      case tag_PIN_MODE:                err = PinMode(stream, pin, value); break;
      case tag_GET_PIN_SERVICES_LIST:   asip.sendPinServicesList();        break;
      case tag_DIGITAL_WRITE:           err = DigitalWrite(pin,value);     break; 
      case tag_ANALOG_WRITE:            err = AnalogWrite(pin,value);      break;
      default:                          err = ERR_UNKNOWN_REQUEST;       
   }
   if( err != ERR_NO_ERROR){
       asip.sendErrorMessage(id_IO_SERVICE, request, err, stream);
   }
}

void asipIOClass::setAnalogPinAutoReport(byte analogPin, boolean report)  // sets pin mode and flag for unsolicited messages 
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

void asipIOClass::setDigitalPinAutoReport(byte pin,boolean report)
{
// todo - add error checking here
   byte portIndex = getPortIndex(pin);
   if( portIndex == PORT_ERROR) {
      VERBOSE_DEBUG(printf( "Unable to get port index for pin %d\n", pin));
      return;
   }
   byte mask = DIGITAL_PIN_TO_MASK(pin);  
   if(report) {
      reportPinMasks[portIndex] |= mask;
    }
    else
      reportPinMasks[portIndex] &= (~mask);
    VERBOSE_DEBUG( printf("reportDigPins: Port index for pin %d is %d, mask=%xX\n", pin, portIndex, reportPinMasks[portIndex]); )
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
    setAnalogPinAutoReport(PIN_TO_ANALOG(pin), mode == ANALOG_MODE ); // turn on/off reporting
  }
  if (IS_PIN_DIGITAL(pin)) {
    if (mode == INPUT_MODE || mode == INPUT_PULLUP_MODE) {
       setDigitalPinAutoReport(pin,true);  // turn on
       sendDigitalPortChanges(stream,true);
    } else {
       setDigitalPinAutoReport(pin,false); // turn off
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
      VERBOSE_DEBUG(  printf("set pin %d to output mode\n", pin);)

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
  if (pin < TOTAL_PINCOUNT  && IS_PIN_PWM(pin)){
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
  if (pin < TOTAL_PINCOUNT  ){
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

   
 



