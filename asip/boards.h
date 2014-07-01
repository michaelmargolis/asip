/*
 * Boards.h -  Arduino Services Interface Protocol (ASIP)
 *  Pin information derived from pins_arduino.h 
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


#ifndef asip_boards_h
#define asip_boards_h

/* macros providing pin information */

// TODO see if this can be gleaned from the distributed pins_arduino.h
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#define IS_PIN_ANALOG(P)        ((P) >= 14 && (P) < 14 + NUM_ANALOG_INPUTS)
#define PIN_TO_ANALOG(P)        (P-14)
#define ANALOG_PIN_TO_DIGITAL   (P+14)
#define SERIAL_RX_PIN            0
#define SERIAL_TX_PIN            1

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) 
#define IS_PIN_ANALOG(p)        ((p) >= 54 && (p) < NUM_DIGITAL_PINS)
#define PIN_TO_ANALOG(P)        (P-54)
#define ANALOG_PIN_TO_DIGITAL   (P+54)
#define SERIAL_RX_PIN            0
#define SERIAL_TX_PIN            1
#define SERIAL1_RX_PIN           19
#define SERIAL1_TX_PIN           18
#define SERIAL2_RX_PIN           17
#define SERIAL2_TX_PIN           16
#define SERIAL3_RX_PIN           15
#define SERIAL3_TX_PIN           14


#elif defined(__AVR_ATmega32U4__)
#define IS_PIN_ANALOG(p)        ((p) >= 18 && (p) < NUM_DIGITAL_PINS)
#define PIN_TO_ANALOG(P)        (P-18)
#define ANALOG_PIN_TO_DIGITAL   (P+18)
#define SERIAL_RX_PIN            0
#define SERIAL_TX_PIN            1

#elif defined(__AVR_ATmega644P__)
#define IS_PIN_ANALOG(p)        ((p) >= 23 && (p) < NUM_DIGITAL_PINS)
#define PIN_TO_ANALOG(P)        (P-24)
#define ANALOG_PIN_TO_DIGITAL   (P+24)
#define SERIAL_RX_PIN            8
#define SERIAL_TX_PIN            9
#define SERIAL1_RX_PIN           10
#define SERIAL1_TX_PIN           11


// Teensy 1.0
#elif defined(__AVR_AT90USB162__)
#define TOTAL_ANALOG_PINS       0
#define TOTAL_PINS              21 // 21 digital + no analog
#define VERSION_BLINK_PIN       6
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        (0)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           (0)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (0)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Teensy 2.0
#elif defined(__AVR_ATmega32U4__) && defined(CORE_TEENSY)
#define TOTAL_ANALOG_PINS       12
#define TOTAL_PINS              25 // 11 digital + 12 analog
#define VERSION_BLINK_PIN       11
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 11 && (p) <= 22)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 5 || (p) == 6)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (((p)<22)?21-(p):11)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Teensy 3.0
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
#define TOTAL_ANALOG_PINS       14
#define TOTAL_PINS              38 // 24 digital + 10 analog-digital + 4 analog
#define VERSION_BLINK_PIN       13
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 34)
#define IS_PIN_ANALOG(p)        (((p) >= 14 && (p) <= 23) || ((p) >= 34 && (p) <= 38))
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 18 || (p) == 19)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (((p)<=23)?(p)-14:(p)-24)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) 
#define SERIAL_RX_PIN            -1  //negative pin numbers if primary comms is over USB
#define SERIAL_TX_PIN            -1 
#define SERIAL1_RX_PIN           0
#define SERIAL1_TX_PIN           1


// Teensy++ 1.0 and 2.0
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              46 // 38 digital + 8 analog
#define VERSION_BLINK_PIN       6
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 38 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= 0 && (p) < MAX_SERVOS)
#define IS_PIN_I2C(p)           ((p) == 0 || (p) == 1)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 38)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)
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

// board name macros
#if defined(__AVR_ATmega168__)
#define CHIP_NAME "ATmega168" 
#elif defined(__AVR_ATmega328__)
#define CHIP_NAME "ATmega328"
#elif defined(__AVR_ATmega328P__)
#define CHIP_NAME "ATmega328P"
#elif defined(__AVR_ATmega32U4__)
#define CHIP_NAME "ATmega32U4"
#elif defined(__AVR_ATmega1280__)
#define CHIP_NAME "ATmega1280"
#elif defined(__AVR_ATmega2560__) 
#define CHIP_NAME "ATmega2560"
#elif defined(__AVR_ATmega644P__)
#define CHIP_NAME "ATmega644P"
#elif defined(__MK20DX128__)
#define CHIP_NAME "MK20DX128"
#elif defined(__MK20DX256__)
#define CHIP_NAME "MK20DX256"
#else 
#define CHIP_NAME "Unrecognized chip"
#endif

#endif 


