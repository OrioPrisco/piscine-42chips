#include <avr/io.h>
#include <util/delay.h>

#define LED1 PB0
#define LED2 PB1
#define LED3 PB2
#define LED4 PB4
#define LEDS PORTB

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

	DDRB |=	  (1 << LED1)
			| (1 << LED2)
			| (1 << LED3)
			| (1 << LED4);//set leds to output mode
	while (1)
	{
		for (char i = 0; i < 4; i++)
		{
			display_leds(1 << i);
			_delay_ms(300);
		}
		for (char i = 2; i > 0; i--)
		{
			display_leds(1 << i);
			_delay_ms(300);
		}
	}
}
