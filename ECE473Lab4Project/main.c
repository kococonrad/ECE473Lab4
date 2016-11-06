/*
 * ECE473Lab4.c
 *
 * Created: 11/2/2016 9:06:00 PM
 * Author : Kody Conrad
 *
	Count controlled via two rotary encoders

	Mode visualized via LED bargraph

	Mode control via Button board

	SW1 = Mode1 = increment 1; SW2 = Mode2 = increment 2; SW3 = Mode3 = increment 4
 */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include<math.h>
#include<stdlib.h>
#include "SPI.h"
#include "RTC_Time.h"
#include "SegmentDisplay.h"
#include "Encoder.h"
#include "main.h"


#define CLOCK_SET 0b00000001
#define CLOCK_LOCK 0b00000010

#define DEBOUNCE_TIME 4 	// Number of button reads until it acknowledge button press

volatile int count = 0;

uint8_t CurMode = 0x01;
static uint8_t lastButton = 0, deb_count = 0;

char ActiveModes = 0x00;
char ColonControl = 0x00;
RTC_Time Time;

ISR(TIMER0_OVF_vect){
	ColonControl ^= 0x01;
	incrementTime(&Time);
	count++;
}

//Input: NULL
//Ouput: Returns the state of the pushbuttons encompassing debounce functionality
uint8_t readButtons(){
	uint8_t result;
	DDRA = 0x00; // Set port A for input

	PORTB = BUTTON_EN; // Enables only the pushbuttons disabling 7segment
	PORTA = 0xFF; // Enable pullup resistors on pushbuttons; active low
	result = PINA ^ 0xFF;
	_delay_us(2); // Delay for reading PIN

	//Debounce buttons on PORTA
	if(result != lastButton){
		lastButton = result;
		deb_count = 0;
		return 0x00;
	}
	if(result == lastButton && deb_count <= DEBOUNCE_TIME) deb_count++; //if the result equals the previous pushed button it iterates the debounceCnt without risk of rolling over
	return (deb_count == DEBOUNCE_TIME) ? result : 0x00; // returns result if debounce is true otherwise returns no pushed Buttons
}

void Timer0Setup(){
	ASSR = (1 << AS0);
	TCNT0 = 0x00;
	//TCCR0 = (1<<CS00)|(1<<CS02); //Prescaling of 128 for real time clock
	TCCR0 = (0<<CS00)|(1<<CS01); //Prescaling of 256 for double time
	TIMSK = (1<<TOIE0); //Timer 0 Overflow flag interrupt enable
	_delay_ms(1000); //Allow crystal stabilization
}

void init_DeviceDependencies(){
	pinOutput(BG_OEPortDir, BG_OEPin);
	pinOutput(BG_REGCLKDir, BG_REGCLK);
	ENC_DDR  |= (1<<ENC_LoadPin)|(1<<ENC_EnPin);
	ENC_PORT |= (1<<ENC_LoadPin)|(1<<ENC_EnPin);
	LEDDECDIR |= DECODER_Mask;
}

void write2Bar(uint8_t val){
	SPDR = val; //Initiates spi transfer
	//waits till done (8 cycles) SPIF (SPI Interrupt Flag) in SPSR Register (SPI Status Register)
	while(bit_is_clear(SPSR, SPIF));

	//Updates bargraph with new values
	PORTD |= 0x04;
	PORTD &= ~0x04;
}

void getMode(){
	uint8_t modetemp= readButtons();
	
	switch (modetemp)
	{
		case CLOCK_SET:
			write2Bar(CLOCK_SET);
			ActiveModes |= CLOCK_SET;
		break;
		case 2:
			write2Bar(2);
			CurMode = 2;
		break;
		case 4:
			write2Bar(4);
			CurMode = 4;
		break;
		default:
		
		break;
	}
}

int main(){
	Timer0Setup();
	init_SPI();
	init_DeviceDependencies();
	getCurrentEncoderStates();
	sei();   //Global Interrupt Enable
	write2Bar(CurMode);
	Time.sec = 20;
	Time.min = 20;
	Time.hour = 1;
	
	while(1){
		set7SegmentDigits_Number(count); // Writes current number to 7seg
		DisplayDigits(ColonControl);
	}
}