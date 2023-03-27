#include <avr/io.h>

//PB0 : LED1
//PD2 : SW1
int main()
{
	DDRD = 0; // all inputs
	DDRB = (1 << PB0); // set LED1 pin to ouptut mode
	while (1)
	{
		if (PIND & (1 << PD2))// id SW1 pin is high (released)
			PORTB = 0;// LED off
		else
			PORTB = (1 << PB0);//LED on
	}
}
