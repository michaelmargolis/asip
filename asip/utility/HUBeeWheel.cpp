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


#include "HUBeeWheel.h" // this is the modified version of the hubee library

HUBeeBMDWheel::HUBeeBMDWheel()
{
}

HUBeeBMDWheel::HUBeeBMDWheel(const byte In1Pin, const byte In2Pin, const byte PWMPin)
{
  IN1 = In1Pin;
  IN2 = In2Pin;
  _PWM= PWMPin;
  STBY = -1;
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  initialise();
}

HUBeeBMDWheel::HUBeeBMDWheel(const byte In1Pin, const byte In2Pin,const byte PWMPin, const byte STBYPin)
{
  IN1 = In1Pin;
  IN2 = In2Pin;
  _PWM = PWMPin;
  STBY = STBYPin;
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  initialise();
}

void HUBeeBMDWheel::setupPins(const byte  In1Pin, const byte  In2Pin, const byte  PWMPin)
{
  IN1 = In1Pin;
  IN2 = In2Pin;
  _PWM= PWMPin;
  STBY = -1;
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  initialise();
}

void HUBeeBMDWheel::setupPins(const byte  In1Pin, const byte  In2Pin, const byte  PWMPin, const byte  STBYPin)
{
  IN1 = In1Pin;
  IN2 = In2Pin;
  _PWM= PWMPin;
  STBY = STBYPin;
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  initialise();
}

void HUBeeBMDWheel::initialise()
{
  motorPower = 0;
  rawPower = 0;
  motorDirection = 1;
  motorBrakeMode = 0;
  motorStandbyMode = 0;
  motorDirectionMode = 0;
  setMotor();
}

void HUBeeBMDWheel::setBrakeMode(boolean brakeMode)
{
  //sets the braking mode
  //0 = freewheeling - motor will freewheel when power is set to zero
  //1 = braking - motor terminals are shorted when power is set to zero, motor will stop quickly
  motorBrakeMode = brakeMode;
}

void HUBeeBMDWheel::setDirectionMode(boolean directionMode)
{
  //set the direction mode
  //setting this to 1 will invert the normal motor direction
  motorDirectionMode = directionMode;
}

boolean HUBeeBMDWheel::getDirectionMode()
{
  //returns the motor direction mode
  return motorDirectionMode;
}

void HUBeeBMDWheel::stopMotor()
{
  //halt the motor using the current braking mode
  analogWrite(_PWM, 0);
  digitalWrite(IN1, motorBrakeMode);
  digitalWrite(IN2, motorBrakeMode);
}


void HUBeeBMDWheel::setStandbyMode(boolean standbyMode)
{
  //set the standby mode if a standby pin has been assigned.
  //1 = standby mode active
  //0 = standby mode inactive
  //invert the value because holding the STBY pin LOW activates standby on the IC
  motorStandbyMode = 1-standbyMode;
  if(STBY>=0)
  {
    digitalWrite(STBY, motorStandbyMode);
  }
}

void HUBeeBMDWheel::setMotorPower(int MPower)
{
  //set the motor - public function
  //costrain the value to what is allowed
  motorPower = constrain(MPower, -255, 255);
  setMotor(); //call private func to actually set the motor
}

void HUBeeBMDWheel::setMotor()
{
  //set the motor - private function
  rawPower = abs(motorPower);
  if(motorPower < 0) motorDirection = 0;
  else motorDirection = 1;

  if(motorPower == 0)
  {
    stopMotor();
    return;
  }
  printf("pwm on pin %d set to %d\n",_PWM, rawPower);
  //write the speed value to PWM
  analogWrite(_PWM, rawPower);
  //XOR the motor Direction and motorDirectionMode boolean values
  /*
    if the motorDirectionMode value is 1 then the output will be inverted
   motorDirection  ^       motorDirectionMode
   0   ^   0 == 0
   1   ^   0 == 1
   
   0   ^   1 == 1
   1   ^   1 == 0
   */
  digitalWrite(IN1,   (motorDirection ^ motorDirectionMode) );
  //invert the direction for the second control line
  digitalWrite(IN2, 1-(motorDirection ^ motorDirectionMode) );
}

int HUBeeBMDWheel::getMotorPower()
{
  //return the motor power value
  return motorPower;
}

// code for HUB-ee encoder

volatile unsigned long prevMicros[2];  // stores the time of the previous reading
long prevCount[2];   // stores the most recent sent count

typedef struct  // holds encoder data captured in interrupt
{
  volatile long count[2];               // encoder pulse count 
  volatile unsigned long pulseWidth[2]; // most recent pulse width
} 
encoderData_t;

encoderData_t encoderData;   // the raw encoder data
encoderData_t encoderCache;  // cached copy to be used outside of interrupt routine

const byte WHEEL1 = 0; // indices for the encoder arrrays
const byte WHEEL2 = 1;


void QEI_wheel_1();  // forward declerations
void QEI_wheel_2();

void encodersBegin() 
{
  pinMode(wheel_1QeiAPin, INPUT_PULLUP);
  pinMode(wheel_2QeiAPin, INPUT_PULLUP);
  pinMode(wheel_1QeiBPin, INPUT_PULLUP);
  pinMode(wheel_2QeiBPin, INPUT_PULLUP);
#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
  attachInterrupt(2, QEI_wheel_1, CHANGE);
  // pin change interrupt for pin 21
  *digitalPinToPCMSK(wheel_2QeiAPin) |= bit (digitalPinToPCMSKbit(wheel_2QeiAPin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(wheel_2QeiAPin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(wheel_2QeiAPin)); // enable interrupt for the group
#else  
  attachInterrupt(1, QEI_wheel_1, CHANGE);
  attachInterrupt(0, QEI_wheel_2, CHANGE);
  #endif
}

void encodersReset()
{
  prevCount[0] = prevCount[1] = 0;
}

void encodersGetData(unsigned long &pulse1,long &count1, unsigned long &pulse2,  long &count2)
{
  noInterrupts();
  memcpy(&encoderCache, &encoderData, sizeof(encoderCache));  // get a copy of the encoder data   
  interrupts();
  pulse1 = encoderCache.pulseWidth[WHEEL1] ;
  count1 = encoderCache.count[WHEEL1] - prevCount[WHEEL1]; 
  pulse2 = encoderCache.pulseWidth[WHEEL2];
  count2 = encoderCache.count[WHEEL2] - prevCount[WHEEL2];  
  
    // store the current encoder counts
  prevCount[WHEEL1] = encoderCache.count[WHEEL1];
  prevCount[WHEEL2] = encoderCache.count[WHEEL2];
}

//wheel 1 quadrature encoder interrupt function
void QEI_wheel_1()
{  
  encoderData.pulseWidth[WHEEL1] = micros() - prevMicros[WHEEL1];
  prevMicros[WHEEL1] = micros(); // store the current time

  //ChA has changed state
  //Check the state of ChA
  if(digitalRead(wheel_1QeiAPin))
  {
    //pin has gone high
    //check chanel B
    if(digitalRead(wheel_1QeiBPin))
    {
      //both are high
      encoderData.count[WHEEL1]--;
      return;
    }
    //ChB is low
    encoderData.count[WHEEL1]++;
    return;
  }
  //if you get here then ChA has gone low, check ChB
  if(digitalRead(wheel_1QeiBPin))
  {
    //ChB is high
    encoderData.count[WHEEL1]++;
    return;
  }
  //if you get here then A has gone low and B is low
  encoderData.count[WHEEL1]--;
}

//wheel 2 quadrature encoder interrupt function
void QEI_wheel_2()
{
  encoderData.pulseWidth[WHEEL2] = micros() - prevMicros[WHEEL2];
  prevMicros[WHEEL2] = micros(); // store the current time

  //ChA has changed state
  //Check the state of ChA
  if(digitalRead(wheel_2QeiAPin))
  {
    //pin has gone high
    //check chanel B
    if(digitalRead(wheel_2QeiBPin))
    {
      //both are high
      encoderData.count[WHEEL2]++;
      return;
    }
    //ChB is low
    encoderData.count[WHEEL2]--; 
    return;
  }
  //if you get here then ChA has gone low, check ChB
  if(digitalRead(wheel_2QeiBPin))
  {
    //ChB is high
    encoderData.count[WHEEL2]--;
    return;
  }
  //if you get here then A has gone low and B is low
  encoderData.count[WHEEL2]++;
}

// only used for pin change interrupts
ISR (PCINT2_vect) // handle pin change interrupt for vector 2
{
   // for now, we assume any interrupt is for wheel 2 -- TODO
   QEI_wheel_2();
}  
