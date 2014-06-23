
#include <asip.h>       // the base class definitions
#include <asipIO.h>     // the core I/O class definition
#include <utility\OtherServices.h> // derived definitions for other services (distance and servo)    
#include <Servo.h>      // needed for the servo service 


#include <Wire.h>     // needed for I2C
#include "asipIMU.h"  // Inertial measurement services 
#include "I2Cdev.h"   // needed for above  
#include "MPU6050.h"
#include "HMC5883L.h"

// some defines for some test hardware
#define SWITCH 13
#define BUTTON 12
#define RGB_RED 11
#define RGB_GREEN 10
#define RGB_BLUE 9
#define LED 6
#define SERVO 5
#define PIEZO 3
#define RELAY 2
#define POT 16
#define HALL 17
#define THERMISTOR 18
#define PHOTOCELL 19

char * sketchName = "TestI2C";
// the order of the following pins is service specific, see the service definition for details
const pinArray_t servoPins[] = {SERVO};  
bool i2cStarted = false; // flag to indcate that i2c started

gyroClass gyro3Axis(id_GYRO_SERVICE); 
AccelerometerClass accelerometer3Axis(id_ACCELEROMETER_SERVICE); 
HeadingClass heading3Axis(id_HEADING_SERVICE);
servoClass servos(id_SERVO_SERVICE, NO_EVENT);

asipServiceClass *services[] = { 
                                 &asipIO, // the core class for pin level I/O
                                 &gyro3Axis,
                                 &accelerometer3Axis,
                                 &heading3Axis,
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

  gyro3Axis.begin(NBR_GYRO_AXIS,startI2C); // I2C services use begin method with nbr of elements (axis) & start callback
  accelerometer3Axis.begin(NBR_ACCEL_AXIS,startI2C); // gyro and accel have x,y,z axis 
  heading3Axis.begin(NBR_MAG_AXIS,startI2C); // 3 raw values, 4th element is the calculated compass heading 
  servos.begin(1,1,servoPins);
  asip.sendPinModes(); // for debug
  asip.sendPortMap();
}

void loop() 
{
  asip.service();
}

bool startI2C( char service)
{
  // for now, all services that call this method require I2C
  if( !i2cStarted) {    
     Wire.begin();
     i2cStarted = true;
  }
  return true; 
}

