/*
 * SegmentDisplay.c
 *
 * Created: 11/4/2016 7:58:56 PM
 *  Author: gbone
 */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include<avr/io.h>
#include "SegmentDisplay.h"
#include "RTC_Time.h"

volatile SegmentDigits digitValues;

void DisplayDigits(uint8_t flashColon){
	DDRA = 0xFF;
	_delay_us(GhostingAdj);
	//Sets port A to output
	if (flashColon == 1)
	{
		PORTB = Colon;
		DigitsPort = ~(A|B);
	}
	else
	{
		PORTB = Colon;
		DigitsPort = (A|B|C);
	}
	_delay_us(GhostingAdj);
	PORTB = Digit1;
	DigitsPort = ~(digitValues.digit1);
	_delay_us(GhostingAdj);
	PORTB = Digit2;
	DigitsPort = ~(digitValues.digit2);
	_delay_us(GhostingAdj);
	PORTB = Digit3;
	DigitsPort = ~(digitValues.digit3);
	_delay_us(GhostingAdj);
	PORTB = Digit4;
	DigitsPort = ~(digitValues.digit4);
	_delay_us(GhostingAdj);
}

uint8_t dec2Segments(uint8_t Number){
	switch (Number)
	{
		case 0:
		return (A+B+C+D+E+F);
		break;
		case 1:
		return (B+C);
		break;
		case 2:
		return (A+B+G+E+D);
		break;
		case 3:
		return (A+B+C+D+G);
		break;
		case 4:
		return (B+C+F+G);
		break;
		case 5:
		return (A+C+D+F+G);
		break;
		case 6:
		return (A+F+E+D+C+G);
		break;
		case 7:
		return (A+B+C);
		break;
		case 8:
		return (A+B+C+D+E+F+G);
		break;
		case 9:
		return (A+B+C+F+G);
		break;
		default:
		return 0xFF;
		break;
	}
}

void set7SegmentDigits(RTC_Time* CurrentTime){
	digitValues.digit4 = dec2Segments(CurrentTime->hour/1000);
	digitValues.digit3 = dec2Segments((CurrentTime->hour/100)%10);
	digitValues.digit2 = dec2Segments((CurrentTime->min/10)%10);
	digitValues.digit1 = dec2Segments(CurrentTime->min%10);
}