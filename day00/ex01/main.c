#include <avr/io.h>

int main()
{
	DDRB |= (1 << PB0); // set LED1 pin to ouptut mode
	PORTB |= (1 << PB0); // set LED1 pin to high
}
