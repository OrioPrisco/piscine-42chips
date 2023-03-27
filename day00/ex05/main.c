#include <avr/io.h>
#include <util/delay.h>

#define LED1 PB0
#define LED2 PB1
#define LED3 PB2
#define LED4 PB4
#define LEDS PORTB
#define SW1  PD2
#define SW2  PD4
#define SWITCHES PIND
#define SW1MASK (1 << SW1)
#define SW2MASK (1 << SW2)

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

int main()
{
	unsigned char i;
	char old_state;

	i = 0;
	old_state = 0;
	DDRD &= ~(SW1MASK | SW2MASK); // set SW1 and SW2 to input
	DDRB |=	  (1 << LED1)
			| (1 << LED2)
			| (1 << LED3)
			| (1 << LED4);//set leds to output mode

	while (1)
	{
		old_state = (SWITCHES & (SW2MASK | SW1MASK));//log sw1 and sw2 state
		_delay_ms(2);//debounce
		while ((SWITCHES & (SW1MASK | SW2MASK)) == old_state)// wait for SW1 or SW2 to change state
			;
		if ((old_state & SW1MASK) && !(SWITCHES & SW1MASK)) // if SW1 was just pressed
		{
			i += 1;
		}
		if ((old_state & SW2MASK) && !(SWITCHES & SW2MASK)) // if SW2 was just pressed
		{
			i -= 1;
		}
		display_leds(i);
	}
}
