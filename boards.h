/* Boards.h - Pin information derived from pins_arduino.h */


#ifndef asip_boards_h
#define asip_boards_h

/* macros providing pin information */

// TODO see if this can be gleaned from the distributed pins_arduino.h
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#define IS_PIN_ANALOG(P)        ((P) >= 14 && (P) < 14 + NUM_ANALOG_INPUTS)
#define PIN_TO_ANALOG(P)        (P-14)
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) 
#define IS_PIN_ANALOG(p)        ((p) >= 54 && (p) < NUM_DIGITAL_PINS)
#define PIN_TO_ANALOG(P)        (P-54)
#elif defined(__AVR_ATmega32U4__)
#define IS_PIN_ANALOG(p)        ((p) >= 18 && (p) < NUM_DIGITAL_PINS)
#define PIN_TO_ANALOG(P)        (P-18)
#else
#error "Analog pin macros not defined in board.h for this chip"
#endif

#define IS_PIN_DIGITAL(P) (P < NUM_DIGITAL_PINS)      // assumes all pins can be used as digital pins
#define PIN_TO_DIGITAL(P) (P) 

#if not defined (digitalPinHasPWM)
#define digitalPinHasPWM digitalPinToTimer
#endif
#define IS_PIN_PWM(P)  (digitalPinHasPWM(P))
#define PIN_TO_PWM(P)  (P)

#endif 


