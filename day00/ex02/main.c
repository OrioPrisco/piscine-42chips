#include <avr/io.h>

int main()
{
	unsigned long loop_count = F_CPU/12; // loop takes 6 cycles
	DDRB |= (1 << PB0); // set LED1 pin to ouptut mode
	while (1)
	{
		unsigned long i = 0;
		while (i < loop_count) // busy loop
		{
			i++;
		}
		PORTB ^= (1 << PB0); // toggle LED1
	}
}
