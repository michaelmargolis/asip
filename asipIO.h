// asipIO.h

#ifndef asipIO_h
#define asipIO_h

#include <Arduino.h>
#include "asip.h"

const unsigned int DEFAULT_ANALOG_EVENT_INTERVAL = 20; // not yet used
const byte MAX_ANALOG_INPUTS = min(NUM_ANALOG_INPUTS, sizeof(unsigned int) *8); // the size of the port mask variable
   
//Core IO service
const char IO_SERVICE    = 'I';   // tag indicating message is for the low level I/O layer
// IO Methods (messages to Arduino)
const char PIN_MODE      = 'P';   // i/o request  to Arduino to set pin mode
const char DIGITAL_WRITE = 'D';   // i/o request  to Arduino is digitalWrite
const char ANALOG_WRITE  = 'A';   // i/o request to Arduino is analogWrite)
// info requests to Arduino
const char ANALOG_DATA_REQUEST     = 'R'; // request analog data events
const char GET_PORT_TO_PIN_MAPPING = 'M'; // gets a list of pins associated with ports 
const char GET_PIN_MODES           = 'p'; // gets a list of pin modes
 
void sendDigitalPortChanges(Stream * stream); // function to send changed digital port data

// IO events (messages from Arduino)
const char PORT_DATA     = 'p';   //  i/o event from Arduino is data on a digital port
const char ANALOG_VALUE  = 'a';   //  i/o event from Arduinois value of an analog pin

// this class derives from the service class but has direct access to all pins
class asipIOClass : public asipServiceClass
{  
public:
   asipIOClass(const char svcId, const char evtId);
   void begin(void); // this class does not use the base class begin arguments
   void reportValues(Stream * stream);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device   
   void processRequestMsg(Stream *stream);
private:
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void reportAnalogPin(byte pin,boolean report);  // sets pin mode and flag for unsolicited messages
   void reportDigitalPin(byte pin,boolean report); // sets pin mode and flag for unsolicited messages
   asipErr_t PinMode(byte pin, int mode);
   asipErr_t AnalogWrite(byte pin, int value);  
   asipErr_t DigitalWrite(byte pin, byte value); 
   void sendDigitalPort(byte portNumber, byte portData,Stream *stream);

    /* analog inputs */
    unsigned int analogInputsToReport; // bitwise array to store pin reporting
    byte nbrActiveAnalogPins;          // the number of pins to report;   
};  


extern asipIOClass asipIO;


#endif