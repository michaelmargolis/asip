
#include <asip.h>       // the base class definitions
#include <asipIO.h>     // the core I/O class definition
#include "utility/robot.h"       // definitions for mirtle services(motor, ir, encoder etc) 
#include "utility/HUBeeWheel.h"
#include <utility/asipDistance.h> // ultrasonics distance sensor
#include <utility/asipServos.h> // derived definitions for servo
#include <Servo.h> // needed for the servo service 

char * sketchName = "Mirtle";
// the order of the following pins is service specific, see the service definition for details

#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
const pinArray_t motorPins[] = {18,19,3,22,23,4};
const pinArray_t bumpPins[] = {12,15};
// note that analog pins are referred to by their digital number (on uno, 15 = analog 1, 16 =analog 2...)
//const pinArray_t irReflectancePins[] = {30,29,31,28}; // first is control, the remainder are used as analog inputs
const pinArray_t irReflectancePins[] = {25,27,24,26}; // first is control, the remainder are used as analog inputs
#else  // defines for standard Mirto 328 V2 board
const pinArray_t motorPins[] = {8,11,5,12,13,6}; 
const pinArray_t bumpPins[] = {9,10};
// note that analog pins are referred to by their digital number (on uno, 15 = analog 1, 16 =analog 2...)
const pinArray_t irReflectancePins[] = {14,15,16,17}; // first is control, the remainder are used as analog inputs
#endif

// encoder pin constants for supported boards are defined in HUBeeWheel.h
const pinArray_t encoderPins[] = {wheel_1QeiAPin,wheel_1QeiBPin, // defined in HUBeeWheel.h
                                  wheel_2QeiAPin,wheel_2QeiBPin};
//declare servo object(s) 
const byte NBR_SERVOS =1;
Servo myServos[NBR_SERVOS];  // create servo objects

const pinArray_t servoPins[] = {18};     // analog pin 4
asipCHECK_PINS(servoPins[NBR_SERVOS]);  // compiler will check if the number of pins is same as number of servos

const byte NBR_DISTANCE_SENSORS = 1;
const pinArray_t distancePins[] = {19};  // analog pin 5 
asipCHECK_PINS(distancePins[NBR_DISTANCE_SENSORS]); //this declaration tests for correct nbr of pin initializers

// create the services
robotMotorClass motors(id_MOTOR_SERVICE, NO_EVENT);
encoderClass encoders(id_ENCODER_SERVICE);
bumpSensorClass bumpSensors(id_BUMP_SERVICE);
irLineSensorClass irLineSensors(id_IR_REFLECTANCE_SERVICE);
asipServoClass asipServos(id_SERVO_SERVICE, NO_EVENT);
asipDistanceClass asipDistance(id_DISTANCE_SERVICE);

// make a list of the created services
asipService services[] = { 
                                 &asipIO, // the core class for pin level I/O
                                 &motors,
                                 &encoders,
                                 &bumpSensors,
                                 &irLineSensors, 
                                 &asipServos,                                 
                                 &asipDistance };

void setup() {
  Serial.begin(ASIP_BAUD);
 // Serial.begin(250000);
  
  asip.begin(&Serial, asipServiceCount(services), services, sketchName); 
  asip.reserve(SERIAL_RX_PIN);  // reserve pins used by the serial port 
  asip.reserve(SERIAL_TX_PIN);  // these defines are in asip/boards.h
  asipIO.begin(); 
  // start the services
  motors.begin(2,6,motorPins); // two motors that use a total of 6 pins  
  encoders.begin(2,4,encoderPins); // two encoders that use a total of 4 pins 
  bumpSensors.begin(2,2,bumpPins);
  irLineSensors.begin(3,4,irReflectancePins); // 3 sensors plus control pin
  asipDistance.begin(NBR_DISTANCE_SENSORS,distancePins); 
  //asipServos.begin(NBR_SERVOS,servoPins,myServos);
  asip.sendPinModes(); // for debug
  asip.sendPortMap(); 

  for(int i=0; i < asipServiceCount(services); i++)
  {
    services[i]->reportName(&Serial); 
    Serial.print(" is service ");
    Serial.write(services[i]->getServiceId());
    Serial.println();  
  }
}

void loop() 
{
  asip.service();
}

  
