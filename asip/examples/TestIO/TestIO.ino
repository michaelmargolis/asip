#include <asip.h>       // the base class definitions
#include <asipIO.h>     // the core I/O class definition
#include <utility\asipServos.h> // derived definitions for servo    
#include <Servo.h>      // needed for the servo service 
#include <utility\asipDistance.h> // ultrasonics distance sensor

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

//declare servo object(s) 
const byte NBR_SERVOS =1;
Servo myServos[NBR_SERVOS];  // create servo objects

char * sketchName = "TestIO";
// the order of the following pins is service specific, see the service definition for details
const pinArray_t servoPins[] = {SERVO};
extern const pinArray_t servoPins[NBR_SERVOS]; //this declaration tests for correct nbr of pin initializers

const byte NBR_DISTANCE_SENSORS = 1;
const pinArray_t distancePins[] = {DISTANCE};
extern const pinArray_t distancePins[NBR_DISTANCE_SENSORS]; //this declaration tests for correct nbr of pin initializers

asipServoClass asipServos(id_SERVO_SERVICE, NO_EVENT);
asipDistanceClass asipDistance(id_DISTANCE_SERVICE);

asipServiceClass *services[] = { 
                                 &asipIO, // the core class for pin level I/O
                                 &asipDistance,
                                 &asipServos };

int nbrServices = sizeof(services) / sizeof(asipServiceClass*);

void setup() {
  while( !Serial); // For leonardo board
  Serial.begin(57600);
 // Serial.begin(250000);
  
  asip.begin(&Serial, nbrServices, (asipServiceClass **)&services, sketchName ); 
  asipIO.begin(); // start the IO service
  asip.registerPinMode(0,RESERVED_MODE,SYSTEM_SERVICE_ID);  // reserve pins used by the serial port 
  asip.registerPinMode(1,RESERVED_MODE,SYSTEM_SERVICE_ID); 
  asipDistance.begin(NBR_DISTANCE_SENSORS,distancePins); 
  asipServos.begin(NBR_SERVOS,servoPins,myServos);
  asip.sendPinModes(); // for debug
  asip.sendPortMap();
}

void loop() 
{
  asip.service();
}


