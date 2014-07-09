/*HUB-ee BMD Arduino Lib
Provides an object for a single motor using the BMD motor driver, includes optional standby control.
Designed for the BMD or BMD-S motor driver which uses the Toshiba TB6593FNG motor driver IC
Created by Creative Robotics Ltd in 2012.
Released into the public domain.
*/

/*
 * this version renames PWM variable to avoid Arduino macro name clash 
 * Michael Margolis Dec 2013
 */

#ifndef HUBeeWheel_H
#define HUBeeWheel_H
#include "Arduino.h"

class HUBeeBMDWheel
{
    public:
        HUBeeBMDWheel();
        HUBeeBMDWheel(const byte In1Pin, const byte In2Pin, const byte PWMPin);
        HUBeeBMDWheel(const byte In1Pin, const byte In2Pin, const byte PWMPin, const byte STBYPin);
        void setupPins(const byte In1Pin, const byte In2Pin, const byte PWMPin);
        void setupPins(const byte In1Pin, const byte In2Pin, const byte PWMPin, const byte STBYPin);
        void setBrakeMode(boolean brakeMode);
        void stopMotor();
        void setStandbyMode(boolean standbyMode);
        void setDirectionMode(boolean direction);
        boolean getDirectionMode();
        void setMotorPower(int MPower);
        int getMotorPower();
    
    private:
        void initialise();
        void setMotor();
        int motorPower;
        int rawPower;
        boolean motorDirection;
        boolean motorBrakeMode;
        boolean motorStandbyMode;
        boolean motorDirectionMode;
        //pin assignments
        byte IN1;
        byte IN2;
        byte _PWM;
        byte STBY;
};

// encoder pins
#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
const byte wheel_1QeiAPin = 2; //external interrupt 2 used for wheel 1 encoder channel A
const byte wheel_1QeiBPin = 1; //wheel 1 encoder channel B input
const byte wheel_2QeiAPin = 21; //pin change interrupt 2 used for wheel 2 encoder channel A
const byte wheel_2QeiBPin = 20; //wheel 2 encoder channel B input
#else
const byte wheel_1QeiAPin = 3; //external interrupt 0 used for wheel 1 encoder channel A
const byte wheel_1QeiBPin = 7; //wheel 1 encoder channel B input
const byte wheel_2QeiAPin = 2; //external interrupt 1 used for wheel 2 encoder channel A
const byte wheel_2QeiBPin = 4; //wheel 2 encoder channel B input
#endif

void encodersBegin();
void encodersGetData(unsigned long &pulse1,long &count1, unsigned long &pulse2,  long &count2);
void encodersReset();

#endif


