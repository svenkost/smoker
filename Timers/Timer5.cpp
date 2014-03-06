/*
 *  Interrupt and PWM utilities for 16 bit Timer4 on ATmega168/328
 *  Original code by Jesse Tane for http://labs.ideo.com August 2008
 *  Modified March 2009 by Jerome Despatis and Jesse Tane for ATmega328 support
 *  Modified June 2009 by Michael Polli and Jesse Tane to fix a bug in setPeriod() which caused the timer to stop
 *  Modified Oct 2009 by Dan Clemens to work with Timer4 of the ATMega1280 or Arduino Mega
 *
 *  This is free software. You can redistribute it and/or modify it under
 *  the terms of Creative Commons Attribution 3.0 United States License.
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#include "Timer5.h"
#define CPU 1600000UL

TimerFive Timer5;              // preinstatiate

//ISR(TIMER4_COMPA_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
ISR(TIMER5_COMPA_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  Timer5.isrCallback();
}

void TimerFive::initialize(long microseconds)
{
  TCCR5A = 0;                 // clear control register A
  TCCR5B = 0;        // set mode as phase and frequency correct pwm, stop the timer
  setPeriod(microseconds);
}
/*
 * Prescale	Time per counter tick	Max Period
 *		1		0.0625 uS			8.192 mS
 *		8		0.5 uS				65.536 mS
 *		64		4 uS				524.288 mS
 *		256		16 uS				2097.152 mS
 * 		1024	64 uS				8388.608 mS
 *
 */
void TimerFive::setPeriod(long microseconds)
{
  // 1 cycle = 1/16.000.000 s
//  long cycles = (F_CPU/2000000) * microseconds;                               // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  // 1 cycle = 64 microseconds because of the 1024 prescaler used
  long cycles;
  cycles = microseconds * 2;
  if (cycles < RESOLUTION) {
	  clockSelectBits = _BV(CS51);
  } else {
	  cycles = microseconds / 4;
	  if (cycles < RESOLUTION) {
		  clockSelectBits = _BV(CS51) | _BV(CS50);
	  } else {
		  cycles = microseconds / 16;
		  if (cycles < RESOLUTION) {
			  clockSelectBits = _BV(CS52);
		  } else {
			  cycles = microseconds / 64;
			  if (cycles > RESOLUTION) {
				  cycles = RESOLUTION-1;
			  }
			  clockSelectBits = _BV(CS52) | _BV(CS50);
		  }
	  }
  }


//  if(cycles < RESOLUTION)              clockSelectBits = _BV(CS40);              // no prescale, full xtal
//  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS41);              // prescale by /8
//  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS41) | _BV(CS40);  // prescale by /64
//  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS42);              // prescale by /256
//  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS42) | _BV(CS40);  // prescale by /1024
//  else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS42) | _BV(CS40);  // request was out of bounds, set as maximum
  OCR5A = pwmPeriod = cycles;
  TCCR5B |= _BV(WGM52);
  TCCR5B &= ~(_BV(CS50) | _BV(CS51) | _BV(CS52));
  TIMSK5 |= _BV(OCIE4A);

  TCCR5B |= clockSelectBits;                                                     // reset clock select register
}


void TimerFive::attachInterrupt(void (*isr)(), long microseconds)
{
  cli();
  if(microseconds > 0) setPeriod(microseconds);
  isrCallback = isr;                                       // register the user's callback with the real ISR
  sei();                                                   // ensures that interrupts are globally enabled
  start();
}

void TimerFive::detachInterrupt()
{
  TIMSK5 &= ~_BV(OCIE5A);                                   // clears the timer overflow interrupt enable bit
}

void TimerFive::start()
{
  TCCR5B |= clockSelectBits;
}

void TimerFive::stop()
{
  TCCR5B &= ~(_BV(CS50) | _BV(CS51) | _BV(CS52));          // clears all clock selects bits
}

void TimerFive::restart()
{
  TCNT5 = 0;
}
