#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LED1 PB0
#define LED2 PB1
#define LED3 PB2
#define LED4 PB4
#define LEDS PORTB
#define LEDS_DDR DDRB
#define LED1MASK (1 << LED1)
#define LED2MASK (1 << LED2)
#define LED3MASK (1 << LED3)
#define LED4MASK (1 << LED4)

#define LEDR PD5
#define LEDG PD6
#define LEDB PD3
#define LEDRGB PORTD
#define LEDRGB_DDR DDRD
#define LEDRMASK (1 << LEDR)
#define LEDGMASK (1 << LEDG)
#define LEDBMASK (1 << LEDB)

#define SW1 PD2
#define SW2 PD4
#define SWITCHES PIND
#define SWS_DDR DDRD
#define SW1MASK (1 << SW1)
#define SW2MASK (1 << SW2)

#define CLOCK_MASK ((1 << CS00)|| ())
#define CLOCK_0    0b000
#define CLOCK_1    0b001
#define CLOCK_8    0b010
#define CLOCK_64   0b011
#define CLOCK_256  0b100
#define CLOCK_1024 0b101

#define PRESCALER				1024L	// need every litle bit of help with 8 bit timer
#define DESIRED_FREQUENCY		1L
#define STEPS					30
#define FREQUENCY_REG			((F_CPU)/((4 * PRESCALER * DESIRED_FREQUENCY * STEPS))) //formula from manual
#define BLINKING_FREQ			10000L //lower means it doesn't look like it's blinking
#define BLINKING_FREQ_STEP		(BLINKING_FREQ/STEPS)

_Bool rising = 0;
ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	if (!rising)
	{
		if (OCR1A < BLINKING_FREQ_STEP)
		{
			OCR1A = 0;
			rising = 1;
		}
		else
			OCR1A -= BLINKING_FREQ_STEP;
	}
	else
	{
		if (OCR1A > BLINKING_FREQ - BLINKING_FREQ_STEP)
		{
			OCR1A = BLINKING_FREQ;
			rising = 0;
		}
		else
			OCR1A += BLINKING_FREQ_STEP;
	}
}

int main()
{

	LEDS_DDR	|= LED2MASK;		// LED2 : out
	SREG		|= (1 << 7);		// enable Interrupts

	//Timer 0
	TCCR0A	|= (1 << WGM00);					//|Mode 5 : PWM Phase correct top : OCRA
	TCCR0B	|= (1 << WGM02);					//|
	OCR0A	= FREQUENCY_REG;					// TOP
	TIMSK0	|= (1 << OCIE0A);					// Enable interrupt on Match A (Timer 0)
	TCCR0B	|= CLOCK_1024;						// set clock prescaler to 1024
	//the timer has now started !

	//Timer 1
	TCCR1A	|= (1 << COM1A0) | (1 << COM1A1);	// toggle on upcounting and downcounting
	TCCR1B	|= (1 << WGM13);					// Mode 8, PWN Phase and frequency correct Top ICR1
	ICR1	= BLINKING_FREQ;					// TOP
	OCR1A	= 0;								// COMPARE, (0% duty cycle at the start)
	TCCR1B	|= CLOCK_1;							// set clock prescaler to 1, fast
	//the timer has now started !
	while (1)
	{

	}
}
