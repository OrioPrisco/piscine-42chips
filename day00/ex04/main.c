#include <avr/io.h>
#include <util/delay.h>

//PB0 : LED1
//PD2 : SW1
int main()
{
	DDRD = 0; // all inputs
	DDRB = (1 << PB0); // set LED1 pin to ouptut mode
	while (1)
	{
		while (PIND & (1 << PD2)) // wait for SW1 to be pushed
			;
		PORTB ^= (1 << PB0); // toggle LED1
		_delay_ms(20); // debounce by waiting 20ms
		while (!(PIND & (1 << PD2))) // wait for SW1 to be released
			;
	}
}
