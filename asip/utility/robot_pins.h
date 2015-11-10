/*
 * Pin allocations for mirto boards
 * The order of the following pins is service specific, see the service definition for details
 */
#ifndef robot_pins_h
#define robot_pins_h

#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
const byte motorPins[] = {18,19,3,22,23,4};
const byte wheel_1QeiAPin    = 2; //external interrupt 2 used for wheel 1 encoder channel A
const byte wheel_1QeiBPin    = 1; //wheel 1 encoder channel B input
const byte wheel_2QeiAPin    = 21; //pin change interrupt 2 used for wheel 2 encoder channel A
const byte wheel_2QeiBPin    = 20; //wheel 2 encoder channel B input
const byte bumpPins[]  = {12,15};
// note that analog pins are referred to by their digital number (on uno, 15 = analog 1, 16 =analog 2...)
//const byte irReflectancePins[] = {30,29,31,28}; // first is control, the remainder are used as analog inputs
const byte irReflectancePins[] = {25,27,24,26}; // first is control, the remainder are used as analog inputs

#elif defined(__MK20DX256__) // Teensy 3.1
const byte motorPins[] = {20,6,25,27,30,32};  // L-in1,L-in2,L-PWM, R-in1,R-in2,R-PWM
const int wheel_1QeiAPin = 29;  //external interrupt 0 used for wheel 1 encoder channel A
const int wheel_1QeiBPin = 31;  //wheel 1 encoder channel B input
const int wheel_2QeiAPin = 28;  //external interrupt 1 used for wheel 2 encoder channel A
const int wheel_2QeiBPin = 26;  //wheel 2 encoder channel B input
const byte bumpPins[] = {24,33};
const byte irReflectancePins[] = {A8,A9,A12,A13}; // first is control, the remainder are used as analog inputs
const byte servoPins[]    = {3};
const byte distancePins[] = {4};
// the following pins use preprocessor defines to enable conditional compile
#define neoPixelPin 2
#define tonePin     9
#define ledPin     13

#else  // defines for standard Mirto 328 V2 board
const byte motorPins[] = {8,11,5,12,13,6}; 
const byte wheel_1QeiAPin    = 3; //external interrupt 0 used for wheel 1 encoder channel A
const byte wheel_1QeiBPin    = 7; //wheel 1 encoder channel B input
const byte wheel_2QeiAPin    = 2; //external interrupt 1 used for wheel 2 encoder channel A
const byte wheel_2QeiBPin    = 4; //wheel 2 encoder channel B input
const byte bumpPins[]  = {9,10};
// note that analog pins are referred to by their digital number (on uno, 15 = analog 1, 16 =analog 2...)
const byte irReflectancePins[] = {14,15,16,17}; // first is control, the remainder are used as analog inputs
const byte servoPins[] = {18};     // analog pin 4
const byte distancePins[] = {19};  // analog pin 5 
#endif

const byte encoderPins[] = {wheel_1QeiAPin,wheel_1QeiBPin,wheel_2QeiAPin,wheel_2QeiBPin};
#endif
