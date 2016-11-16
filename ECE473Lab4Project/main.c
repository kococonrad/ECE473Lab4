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
#include "WaveGeneration.h"
#include "Encoder.h"
#include "main.h"


#define CLOCK_SET 0b00000001
#define ALARM_SET 0b00000010

#define DEBOUNCE_TIME 4 	// Number of button reads until it acknowledge button press

volatile int count = 0;
volatile uint8_t brightness=0;

uint8_t CurMode = 0x01;
uint8_t frequency = 0;
static uint8_t lastButton = 0, deb_count = 0;

char ActiveModes = 0x00;
char ColonControl = 0x00;
RTC_Time Time;
RTC_Time AlarmTime;
uint8_t AlarmArmed = 0;

int scale[] = {C5, CSharp5, D5, DSharp5, E5, F5, FSharp5, G5, GSharp5, A5, ASharp5, B5};

ISR(TIMER0_OVF_vect){
	if (frequency > 11)
	{
		frequency = 0;	
	}
	else if (frequency < 0)
	{
		frequency = 0;
	}
	ColonControl ^= 0x01;
	incrementTime(&Time);
	setFrequency(scale[frequency]);
	frequency++;
	count++;
}

ISR(ADC_vect){
	setBrightness(ADCH);
}

//Input: NULL
//Ouput: Returns the state of the pushbuttons encompassing debounce functionality
uint8_t readButtons(){
	uint8_t result;
	DDRA = 0x00; // Set port A for input

	DigitSelectPort = BUTTON_EN; // Enables only the pushbuttons disabling 7segment
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
	TCCR0 = (1<<CS00)|(1<<CS02); //Prescaling of 128 for real time clock
	//TCCR0 = (0<<CS00)|(1<<CS01); //x4 Clock speed
	TIMSK = (1<<TOIE0); //Timer 0 Overflow flag interrupt enable
	_delay_ms(1000); //Allow crystal stabilization
}



void init_DeviceDependencies(){
	pinOutput(BG_OEPortDir, BG_OEPin);
	pinOutput(BG_REGCLKDir, BG_REGCLK);
	ENC_DDR  |= (1<<ENC_LoadPin)|(1<<ENC_EnPin);
	ENC_PORT |= (1<<ENC_LoadPin)|(1<<ENC_EnPin);
	DigitSelectDDR |= DECODER_Mask; //Initilization for DDRC segment display decoder.
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
	DDRC = 0x03FF;
	switch (modetemp)
	{
		case CLOCK_SET:
			write2Bar(CLOCK_SET);
			Time.sec = DisplayTime_TimeSetter(&Time, S);
			Time.min = DisplayTime_TimeSetter(&Time, M);
			Time.hour = DisplayTime_TimeSetter(&Time, H);
		break;
		case ALARM_SET:
			write2Bar(ALARM_SET);
			AlarmTime.sec = DisplayTime_TimeSetter(&Time, S);
			AlarmTime.min = DisplayTime_TimeSetter(&Time, M);
			AlarmTime.hour = DisplayTime_TimeSetter(&Time, H);
		break;
		default:
			write2Bar(Time.sec);
			DisplayTime(&Time, ColonControl);
		break;
	}
}

void SoundAlarm(){
	AlarmDisplayTime(&Time, ColonControl);
	_delay_ms(10);
}

int main(){
	Timer0Setup();
	timer1_init();
	Timer2Setup();
	init_DeviceDependencies();
	Timer3Setup();
	init_SPI();
	ADC0Setup();
	
	getCurrentEncoderStates();
	sei();   //Global Interrupt Enable
	write2Bar(CurMode);
	Time.sec = 45;
	Time.min = 33;
	Time.hour = 0;
	AlarmTime.sec = 0;
	AlarmTime.min = 0;
	AlarmTime.hour = 0;
	
	while(1){
		setVolume(0);
		
		getMode();
		if (AlarmArmed == 1)
		{
			if (AlarmTime.hour == Time.hour && AlarmTime.min == Time.min)
			{
				SoundAlarm();
			}
		}
	}
}