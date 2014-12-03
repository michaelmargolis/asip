/* A board with a distance sensor and a NeoPixel strip with 60 LEDs */

#include <asip.h>       // the base class definitions
#include <asipIO.h>     // the core I/O class definition
#include <utility/asipDistance.h> // ultrasonics distance sensor
#include <Adafruit_NeoPixel.h> // NeoPixel library
#include <utility/asipNeoPixels.h> // NeoPixel ASIP library
#include <Servo.h>

#define DISTANCE 4
#define NP_STRIP1 6
#define NP_STRIP2 9


//declare NeoPixel object(s) 
const byte NBR_STRIPS =2;
Adafruit_NeoPixel myStrips[NBR_STRIPS] = {  Adafruit_NeoPixel(8, NP_STRIP1, NEO_GRB + NEO_KHZ800), Adafruit_NeoPixel(16, NP_STRIP2, NEO_GRB + NEO_KHZ800) };  // create strip objects

// Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

char * sketchName = "neoPixels";
// the order of the following pins is service specific, see the service definition for details
const pinArray_t stripPins[] = { NP_STRIP1, NP_STRIP2 };
extern const pinArray_t stripPins[NBR_STRIPS];
//asipCHECK_PINS(stripPins[NBR_STRIPS]);  // compiler will check if the number of pins is same as number of strips

const byte NBR_DISTANCE_SENSORS = 1;
const pinArray_t distancePins[] = {DISTANCE};
extern const pinArray_t distancePins[NBR_DISTANCE_SENSORS]; //this declaration tests for correct nbr of pin initializers

asipNeoPixelsClass asipNeoPixels(id_NEOPIXELS_SERVICE, NO_EVENT);
asipDistanceClass asipDistance(id_DISTANCE_SERVICE);

asipServiceClass *services[] = { 
                                 &asipIO, // the core class for pin level I/O
                                 &asipDistance,
                                 &asipNeoPixels };

int nbrServices = sizeof(services) / sizeof(asipServiceClass*);

void setup() {
  while( !Serial); // For leonardo board
  Serial.begin(57600);
 // Serial.begin(250000);
  
  asip.begin(&Serial, nbrServices, (asipServiceClass **)&services, sketchName ); 
  asipIO.begin(); // start the IO service
  asip.reserve(SERIAL_RX_PIN);  // reserve pins used by the serial port 
  asip.reserve(SERIAL_TX_PIN);  // these defines are in asip/boards.h 
  asipDistance.begin(NBR_DISTANCE_SENSORS,distancePins); 
  asipNeoPixels.begin(NBR_STRIPS,stripPins,myStrips);
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


