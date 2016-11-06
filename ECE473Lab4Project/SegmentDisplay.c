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
	uint8_t zeroState = 0x00;
	DDRA = 0xFF;
	PORTB = Digit4;
	if (digitValues.digit4 == Zero)
	{
		zeroState |= Digit4ZeroBit;
		DigitsPort = 0xFF;
	}
	else
		DigitsPort = ~(digitValues.digit4);
	_delay_us(GhostingAdj);
	PORTB = Digit3;
	if (digitValues.digit3 == Zero && zeroState == Digit4ZeroBit)
	{
		zeroState |= Digit3ZeroBit;
		DigitsPort = 0xFF;
	}
	else
		DigitsPort = ~(digitValues.digit3);
	_delay_us(GhostingAdj);
	PORTB = Digit2;
	if (digitValues.digit2 == Zero && zeroState == D4_D3Zero)
	{
		zeroState |= Digit2ZeroBit;
		DigitsPort = 0xFF;
	}
	else
		DigitsPort = ~(digitValues.digit2);
	_delay_us(GhostingAdj);
	PORTB = Digit1;
	DigitsPort = ~(digitValues.digit1);
	_delay_us(GhostingAdj);
	//Sets port A to output
	if (flashColon == 1)
	{
		PORTB = Colon;
		DigitsPort = ~(A|B|~C);
	}
	else
	{
		PORTB = Colon;
		DigitsPort = (A|B|C);
	}
	_delay_us(GhostingAdj);
	DigitsPort = 0xFF;
}

uint8_t dec2Segments(uint8_t Number){
	switch (Number)
	{
		case 0:
		return Zero;
		break;
		case 1:
		return One;
		break;
		case 2:
		return Two;
		break;
		case 3:
		return Three;
		break;
		case 4:
		return Four;
		break;
		case 5:
		return Five;
		break;
		case 6:
		return Six;
		break;
		case 7:
		return Seven;
		break;
		case 8:
		return Eight;
		break;
		case 9:
		return Nine;
		break;
		default:
		return 0xFF;
		break;
	}
}

void set7SegmentDigits_Time(RTC_Time* CurrentTime){
	digitValues.digit4 = dec2Segments(CurrentTime->hour/1000);
	digitValues.digit3 = dec2Segments((CurrentTime->hour/100)%10);
	digitValues.digit2 = dec2Segments((CurrentTime->min/10)%10);
	digitValues.digit1 = dec2Segments(CurrentTime->min%10);
}

void set7SegmentDigits_Number(int Number){
	digitValues.digit4 = dec2Segments(Number/1000);
	digitValues.digit3 = dec2Segments((Number/100)%10);
	digitValues.digit2 = dec2Segments((Number/10)%10);
	digitValues.digit1 = dec2Segments(Number%10);
}
