#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

int main()
{
	DDRB |= (1 << PB0);//led0
	uart_init();

	while (1)
	{
		uart_tx('Z');
		PORTB ^= (1 << PB0);
		_delay_ms(1000);
	}
}
