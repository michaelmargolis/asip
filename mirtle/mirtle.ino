
#include <asip.h>       // the base class definitions
#include <asipIO.h>     // the core I/O class definition
#include <robot.h>       // derived definitions for mirtle services(motor, ir, encoder etc) 
#include <HUBeeWheel.h>
#include <OtherServices.h> // derived definitions for other services (distance and servo) 

#include <Servo.h> // needed for the servo service 

char * sketchName = "Mirtle";

// the order of the following pins is service specific, see the service definition for details
const pinArray_t motorPins[] = {8,11,9,12,13,10};
const pinArray_t encoderPins[] = {wheel_1QeiAPin,wheel_1QeiBPin, // defined in HUBeeWheel.h
                                  wheel_2QeiAPin,wheel_2QeiBPin}; 
const pinArray_t bumpPins[] = {6,5};
// note that analog pins are referred to by their digital number (on uno, 15 = analog 1, 16 =analog 2...)
const pinArray_t irReflectancePins[] = {14,15,16,17}; // first is control, the remainder are used as analog inputs

const pinArray_t servoPins[] = {18};     // analog pin 4
const pinArray_t distancePins[] = {19};  // analog pin 5 

asipServiceClass *services[] = { 
                                 &asipIO, // the core class for pin level I/O
                                 &motors,
                                 &encoders,
                                 &bumpSensors,
                                 &irLineSensors, 
                                 &servos,                                 
                                 &distanceSensor };

int nbrServices = sizeof(services) / sizeof(asipServiceClass*);

void setup() {
  Serial.begin(57600);
 // Serial.begin(250000);
  
  asip.begin(&Serial, nbrServices, (asipServiceClass **)&services, sketchName); 
  asipIO.begin(); 
  asip.registerPinMode(0,RESERVED_MODE);  // block the serial pins from being allocated
  asip.registerPinMode(1,RESERVED_MODE);
  motors.begin(2,6,motorPins); // two motors that use a total of 6 pins  
  encoders.begin(2,4,encoderPins); // two encoders that use a total of 4 pins 
  bumpSensors.begin(2,2,bumpPins);
  irLineSensors.begin(3,4,irReflectancePins); // 3 sensors plus control pin
  distanceSensor.begin(1,1,distancePins);
  //servos.begin(1,1,servoPins);
  Serial.println("ready");
  asip.sendPinModes(); // for debug
  asip.sendPinMap(); 
}

void loop() 
{
  asip.service();
}

  
