#include <asip.h>       // the base class definitions
#include <asipIO.h>     // the core I/O class definition
#include <utility\OtherServices.h> // derived definitions for other services (distance and servo)    
#include <Servo.h>      // needed for the servo service 

// some defines for some test hardware
#define SWITCH 13
#define BUTTON 12
#define RGB_RED 11
#define RGB_GREEN 10
#define RGB_BLUE 9
#define LED 6
#define SERVO 5
#define DISTANCE 4
#define PIEZO 3
#define RELAY 2
#define POT 16
#define HALL 17
#define THERMISTOR 18
#define PHOTOCELL 19

char * sketchName = "TestIO";
// the order of the following pins is service specific, see the service definition for details
const pinArray_t servoPins[] = {SERVO};
const pinArray_t distancePins[] = {DISTANCE}; 

servoClass servos(id_SERVO_SERVICE, NO_EVENT);
distanceSensorClass distanceSensor(id_DISTANCE_SERVICE);

asipServiceClass *services[] = { 
                                 &asipIO, // the core class for pin level I/O
                                 &distanceSensor,
                                 &servos };

int nbrServices = sizeof(services) / sizeof(asipServiceClass*);

void setup() {
  while( !Serial); // For leonardo board
  Serial.begin(57600);
 // Serial.begin(250000);
  
  asip.begin(&Serial, nbrServices, (asipServiceClass **)&services, sketchName ); 
  asipIO.begin(); // start the IO service
  asip.registerPinMode(0,RESERVED_MODE);  // reserver pins used by the serial port 
  asip.registerPinMode(1,RESERVED_MODE); 
  distanceSensor.begin(1,1,distancePins); 
  servos.begin(1,1,servoPins);
  asip.sendPinModes(); // for debug
  asip.sendPortMap();
}

void loop() 
{
  asip.service();
}


