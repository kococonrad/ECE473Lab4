/*
 * main.h
 *
 * Created: 11/6/2016 12:25:16 PM
 *  Author: gbone
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define PinToggle(PORTX, pin) PORTX ^= (1<<pin)
#define PinHIGH(PORTX, pin) PORTX |= (1<<pin)
#define PinLOW(PORTX, pin) PORTX &= ~(1<<pin)
#define pinInput(DDRX, pin) DDRX &= ~(1<<pin)
#define pinOutput(DDRX, pin) DDRX |= (1<<pin)
#define port2Input(DDRX) DDRX = 0x00;
#define port2Output(DDRX) DDRX = 0xFF;

#define PWMPin PB7
#define PWMPort PORTB
#define PWMDDR DDRB

uint8_t readButtons();

#endif /* MAIN_H_ */