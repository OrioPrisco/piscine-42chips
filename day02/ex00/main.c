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

#define SW1 PD2
#define SW2 PD4
#define SWS_DDR DDRD
#define SW1MASK (1 << SW1)
#define SW2MASK (1 << SW2)

_Bool pressed = 0;

ISR(INT0_vect, ISR_BLOCK)
{
	pressed ^= 1;
	if (pressed)
		LEDS ^= LED1MASK;		// Toggle LED1
	_delay_ms(2);				// Debounce
	EIFR	|= (1 << INTF0);	// Clear Interrupt request flag
	//It might have been triggered during the delay
}

//sw1 is on INT0
int main()
{
	LEDS_DDR	|= LED1MASK;	// LED1 : out
	SWS_DDR		&= ~SW1MASK;	// SW1  : in
	EIMSK		|= (1 << INT0);	// enable INTerrupt 0 (SW1)
	SREG		|= (1 << 7);	// enable Interrupts
	EICRA		|= (1 << ISC00);// enable INT0(SW1) on logical change

	while (1)
	{

	}
}
