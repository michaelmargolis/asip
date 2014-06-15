// OtherServices.cpp
#include "OtherServices.h"

//declare servo object(s) 
const byte NBR_SERVOS = 1;
Servo myServos[NBR_SERVOS];  // create one servo object


servoClass servos(SERVO_SERVICE, NO_EVENT);

servoClass::servoClass(const char svcId, const char evtId)
  :asipServiceClass(svcId,evtId)
{}

// each servo uses 1 pin
 void servoClass::begin(byte nbrElements, byte pinCount, const pinArray_t pins[])
{
  asipServiceClass::begin(nbrElements,pinCount,pins);
  for(int i=0; i < nbrElements; i++) {     
     myServos[i].attach(pins[i]);
	 printf("Attaching servo id %d to pin %d\n", i, pins[i]);
  } 
}

void servoClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
   // this class does not report
}

 void servoClass::reportValues(Stream *stream)
{
  // this class does not report
}
  
void servoClass::write(byte servoId, byte angle)
{
   if(servoId < NBR_SERVOS){
       angle = constrain(angle,0,180);
       myServos[servoId].write(angle);
	   printf("Servo id %d on pin %d moving to %d degrees\n", servoId, pins[servoId], angle);
   }
}
   
void servoClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   int angle;
   if(request == SERVO_WRITE) {
     int servoId = 0;  
	 if(NBR_SERVOS == 0) {
       reportError(ServiceId, request, ERR_DEVICE_NOT_AVAILABLE, stream);
     }
     else if(NBR_SERVOS > 0) {
       // must send servo id even if only one servo
        servoId = stream->parseInt(); // a single servo has an ID of 0;
        if(servoId > NBR_SERVOS-1) {
          reportError(ServiceId, request, ERR_INVALID_DEVICE_NUMBER, stream);
        }  
        else {
          int angle = stream->parseInt();		  
          write(servoId, angle);		 
        }				
     }

   } 
   else {
      reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }   
}

distanceSensorClass distanceSensor(DISTANCE_SERVICE, DISTANCE_EVENT);

distanceSensorClass::distanceSensorClass(const char svcId, const char evtId) : asipServiceClass(svcId,evtId){}

// each sensor uses 1 pins
void distanceSensorClass::begin(byte nbrElements, byte pinCount, const pinArray_t pins[])
{
  asipServiceClass::begin(nbrElements,pinCount,pins);

}

 void distanceSensorClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
  if( sequenceId < nbrElements) {
       stream->print(getDistance(sequenceId));
  }
}

void distanceSensorClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   if( request == DISTANCE_REQUEST) {
      setAutoreport(stream);
   }
   else if(request == DISTANCE_MEASURE){ 
      reportValues(stream);  // send a single measurement
   }
      
   else {
     reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}

int distanceSensorClass::getDistance(int sequenceId)
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

 