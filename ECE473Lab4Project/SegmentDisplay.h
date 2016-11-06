/*
 * SegmentDisplay.h
 *
 * Created: 11/4/2016 7:59:28 PM
 *  Author: gbone
 */ 
#ifndef SEGMENTDISPLAY_H_
#define SEGMENTDISPLAY_H_
#include<stdint.h>
#include<avr/io.h>
#include<util/delay.h>
#include "RTC_Time.h"

//7Segment definitions
#define Digit1 0x00
#define Digit2 0x10
#define Digit3 0x30
#define Digit4 0x40
#define Colon 0x20
#define DigitsPort PORTA
#define A 0x01 //green
#define B 0x02 //blue
#define C 0x04 //purple
#define D 0x08 //purple?
#define E 0x10 //white
#define F 0x20 //black
#define G 0x40 //brown
#define DEC 0x80 //red

#define LED_ON 1 		// loops this many time through 7 seg
#define BUTTON_EN 0xF0
#define DECODER_Mask 0xF0
#define LEDDECDIR DDRB
#define GhostingAdj 300

typedef struct {
	uint8_t digit1;
	uint8_t digit2;
	uint8_t digit3;
	uint8_t digit4;
	uint8_t colon;
} SegmentDigits;

void DisplayDigits(uint8_t flashColon);
uint8_t dec2Segments(uint8_t Number);
void set7SegmentDigits(RTC_Time* CurrentTime);
#endif /* SEGMENTDISPLAY_H_ */