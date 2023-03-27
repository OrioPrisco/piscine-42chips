#include "gpio.h"

void	display_leds(unsigned char i)
{
	unsigned char leds = LEDS;
	leds &= ~(
		  (1 << LED1)
		| (1 << LED2)
		| (1 << LED3)
		| (1 << LED4)
		); // reset leds
	leds |=
		  ((i >> 0) % 2) << LED1
		| ((i >> 1) % 2) << LED2
		| ((i >> 2) % 2) << LED3
		| ((i >> 3) % 2) << LED4
	; //set leds to display i
	LEDS = leds;
}
