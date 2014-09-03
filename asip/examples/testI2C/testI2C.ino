
#include <asip.h>       // the base class definitions
#include <asipIO.h>     // the core I/O class definition
#include <utility/asipServos.h> // derived definitions for servo  
#include <Servo.h>      // needed for the servo service 

#include <Wire.h>     // needed for I2C
#include "asipIMU.h"  // Inertial measurement services 
#include "I2Cdev.h"   // needed for above  
#include "MPU6050.h"
#include "HMC5883L.h"
#include "BMP085.h"

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

//declare servo object(s) 
const byte NBR_SERVOS =1;
Servo myServos[NBR_SERVOS];  // create servo objects

// the order of the following pins is service specific, see the service definition for details
const pinArray_t servoPins[] = {SERVO}; 
asipCHECK_PINS(servoPins[NBR_SERVOS]);  // compiler will check if the number of pins is same as number of servos

bool i2cStarted = false; // flag to indcate that i2c started

gyroClass gyro3Axis(id_GYRO_SERVICE); 
AccelerometerClass accelerometer3Axis(id_ACCELEROMETER_SERVICE); 
HeadingClass heading3Axis(id_HEADING_SERVICE);
PressureClass pressure(id_PRESSURE_SERVICE);

asipServoClass asipServos(id_SERVO_SERVICE, NO_EVENT);

asipServiceClass *services[] = { 
                                 &asipIO, // the core class for pin level I/O
                                 &gyro3Axis,
                                 &accelerometer3Axis,
                                 &heading3Axis,
                                 &pressure,
                                 &asipServos };

int nbrServices = sizeof(services) / sizeof(asipServiceClass*);

void setup() {
  while( !Serial); // For leonardo board
  Serial.begin(57600);
 // Serial.begin(250000);
  
  asip.begin(&Serial, nbrServices, (asipServiceClass **)&services, sketchName ); 
  asipIO.begin(); // start the IO service
  asip.reserve(SERIAL_RX_PIN);  // reserve pins used by the serial port 
  asip.reserve(SERIAL_TX_PIN);  // these defines are in asip/boards.h 

  gyro3Axis.begin(NBR_GYRO_AXIS,startI2C); // I2C services use begin method with nbr of elements (axis) & start callback
  accelerometer3Axis.begin(NBR_ACCEL_AXIS,startI2C); // gyro and accel have x,y,z axis 
  heading3Axis.begin(NBR_MAG_AXIS,startI2C); // 3 raw values, 4th element is the calculated compass heading 
  pressure.begin(NBR_PRESSURE_FIELDS,startI2C); // pressure, tempearture and altitude
  
  asipServos.begin(NBR_SERVOS,servoPins,myServos);
  asip.sendPinModes(); // for debug
  asip.sendPortMap();
  for(int i=0; i< nbrServices; i++)
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

bool startI2C( char service)
{
  // for now, all services that call this method require I2C
  if( !i2cStarted) {  
     asip.reserve(SDA);  // reserve pins used by I2C 
     asip.reserve(SCL);  // these defines are in pins_arduino.h  
     Wire.begin();
     i2cStarted = true;
  }
  return true; 
}

