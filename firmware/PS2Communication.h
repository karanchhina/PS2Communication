/*
  PS2Communication.h - PS2Communication library
  Copyright (c) 2009 Free Software Foundation.  All right reserved.
  Rewritten for interrupt and ported for Spark Core
  by Andreas Rothenwänder <scruff.r@sbg.at>
  based on some non-interrupt library from pjrc.com (Paul Stoffregen)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef PS2Communication_h
#define PS2Communication_h

#if defined(SPARK)

#include "application.h"

// some very useful macros for Spark Core and porting Arduino libraries for it

// fast pin access
#define pinLO(_pin)	                       (PIN_MAP[_pin].gpio_peripheral->BRR = PIN_MAP[_pin].gpio_pin)
#define pinHI(_pin)	                       (PIN_MAP[_pin].gpio_peripheral->BSRR = PIN_MAP[_pin].gpio_pin)
#define pinSet(_pin, _hilo)                (_hilo ? pinHI(_pin) : pinLO(_pin))
#define pinGet(_pin)                       (PIN_MAP[_pin].gpio_peripheral->IDR & PIN_MAP[_pin].gpio_pin ? 0xFF : LOW)

// even faster port based multi pin access
#define portSet(_port, _word)              (_port->ODR = _word)
#define portSetMasked(_port, _word, _mask) (_port->BSRR = (_mask << 16) | (_word & _mask))

// Arduino porting/replacement macros
#define pgm_read_byte(_addr)               (*(const uint8_t *)(_addr))
#define pgm_read_byte_near(_addr)          (pgm_read_byte(_addr))
#define pgm_read_word(_addr)               (*(const uint16_t *)(_addr))
#define pgm_read_word_near(_addr)          (pgm_read_word(_addr))
#define digitalPinToBitMask(_pin)          (PIN_MAP[_pin].gpio_pin)
#define digitalPinToPort(_pin)             (PIN_MAP[_pin].gpio_peripheral)
#define portInputRegister(_port)           (_port->IDR)
#define portOutputRegister(_port)          (_port->ODR)
#define cbi(_pin)                          pinLO(_pin)
#define sbi(_pin)                          pinHI(_pin)
#define bitRead(_val, _bit)                (_val & (1 << _bit))
#define bitWrite(_dest, _bit, _src)        (_dest |= (_src ? (1 << _bit) : 0))

#define PS2_DATAPIN D0        // needs to be 5V toletant
#define PS2_CLKPIN  D1        // needs to be 5V tolerant & interrupt enabled

// interrupt for PS/2 communication
#define PS2_INTERRUPT PS2_CLKPIN

#else
#include "WProgram.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define pinSet(_pin, _hilo)                digitalWrite(_pin, _hilo)
#define pinGet(_pin)                       digitalRead(_pin)

// PIN for pin change interrupt PCINT0
//#define PS2_CLKPIN 3        // for arduino
//#define PS2_DATAPIN 10
#define PS2_CLKPIN  PIN_D1  // Teensy 1.0
#define PS2_DATAPIN PIN_B0  // Teensy 1.0

// interrupt for PS/2 communication
#define PS2_INTERRUPT 1

#endif

#define WAIT4PS2REPLY  50               // time to transmit a PS2 "byte"

// direction of communication
#define DEV2HOST 0
#define HOST2DEV -1

extern volatile uint8_t ps2InBufferHead;
extern volatile uint8_t ps2InBufferTail;
extern volatile uint16_t inByte, outByte;

class PS2Communication
{
private:
    // sets the line states for clock or data
    //   high .. pin becomes input with pullup resistor
    //   low  .. pin becomes output LOW
    inline void setPin(int pin, uint8_t state);

public:
    // constructor does the pin setup and attaches the interrupt
  	PS2Communication();

    // does the pin setup and attaches the interrupt via reset()
    void begin();

    // indicates whether there is any new data (TRUE) or not (FALSE)
    uint8_t available();

    // returns the next FIFO byte read of the PS2 device
    uint8_t read();

    // sends one byte to the PS/2 device
    void write(uint8_t cmd);

    // flush the in/out buffers
    void flush();
    void flush(int buffer);

    // resets the PS/2 device by pulling clock and data low
    void reset();

    // suspends communication by pulling clock low
    void suspend();

    // resumes communication by releasing clock line
    void resume();

    // host requests to send data to the device
    void rts();
};
#endif