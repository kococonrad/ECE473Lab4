/*
 * WaveGeneration.c
 *
 * Created: 11/6/2016 10:44:54 PM
 *  Author: gbone
 */ 
/// Use this once to initialize, call only once.
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include<avr/io.h>
#include "main.h"

 void timer1_init()
 {
  /// Set PORTD5 as output
   DDRB |= 0b00100000;

  // You might want to initialize registers here instead.
 }
/// Set to the given frequency
 void setFrequency(int frequency)
 {
  /// Disable timer at 0Hz
   if (frequency == 0)
   {
    TCCR1A = 0;
    TCCR1B = 0;
    return;
   }

  /// Set settings registers
   /* The settings you will be interested in:
    WGM11|WGM12|WGM13 = FAST PWM mode ICR1 as top, see datasheet page 114
    CS10 = No prescaler, you might want to change cs10-12 to create a slower acting generator. See datasheet page 115
   */
   TCCR1A |= (1<<WGM11)|(1<<COM1A1);
   TCCR1B |= (1<<WGM12)|(1<<WGM13)|(1<<CS10);

  /// Write frequency
   frequency = F_CPU / frequency; // Note that 1 / F = t. F_CPU should be the timer frequency, which is in this case the same as F_CPU
   ICR1 = frequency; // Set max to the period
   OCR1A = frequency >> 1; // Set compare to half the period
 }
 
 void Timer3Setup(){
	 TCCR3A = (1<<WGM31)|(1<<WGM30)|(1<<COM3A1)|(0<<COM3A0);
	 TCCR3B = (0<<WGM33)|(1<<WGM32)|(0<<CS31)|(1<<CS30);
	 pinOutput(DDRE, PE3);
 }

 void setVolume(uint16_t duty){
	 OCR3A = duty;
 }