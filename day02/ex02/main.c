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

unsigned char i = 0;

void	display_leds(unsigned char i)
{
	LEDS &= ~( 
		  (1 << LED1)
		| (1 << LED2)
		| (1 << LED3)
		| (1 << LED4)
		); // reset leds
	LEDS |=
		  ((i >> 0) % 2) << LED1
		| ((i >> 1) % 2) << LED2
		| ((i >> 2) % 2) << LED3
		| ((i >> 3) % 2) << LED4
	; //set leds to display i
}

ISR(INT0_vect, ISR_BLOCK)
{
	_delay_ms(2);				// Debounce
	if (!(SWITCHES & SW1MASK))
		i++;
	display_leds(i);
	EIFR	|= (1 << INTF0);	// Clear Interrupt request flag
	//It might have been set during the delay
}

ISR(PCINT2_vect, ISR_BLOCK)
{
	_delay_ms(2);				//Debounce
	if (!(SWITCHES & SW2MASK))
		i--;
	display_leds(i);
	PCIFR	|= (1 << PCIF2);	// Clear interrupt request flag
	//It might have been set during the delay
}

int main()
{
	LEDS_DDR	|= 0
					| LED1MASK
					| LED2MASK
					| LED3MASK
					| LED4MASK
				;				// Enables Leds
	//SW1 INT
	SWS_DDR		&= ~SW1MASK;	// SW1 : in
	EIMSK		|= (1 << INT0);	// enable INTerrupt 0 (SW1)
	EICRA		|= (1 << ISC00);// enable INT0(SW1) on logical change
	//SW2 INT
	SWS_DDR		&= ~SW2MASK;	// SW2 : in
	PCICR		|= (1 << PCIE2);// enable PCINT2
	PCMSK2		|= (1 << PCINT20);// set the PCINT2 Mask to accept PCINT20, (PD4/SW2)

	SREG		|= (1 << 7);	// enable Interrupts
	while (1)
	{

	}
}

