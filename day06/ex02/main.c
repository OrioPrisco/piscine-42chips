#include <util/delay.h>
#include "gpio.h"
#include "uart.h"
#include "utils.h"
#include <avr/interrupt.h>

char rgb[7];
int index_str = 0;

ISR(USART_RX_vect, ISR_BLOCK)
{
	char c = UDR0;

	rgb[index_str] = c;
	uart_tx(c);
	if ( (index_str && !is_hex(c))
		|| (!index_str && c != '#'))
	{
		index_str = 0;
		uart_printstr("\r       \r");
	}
	else
		index_str++;
	if (index_str == 7)
	{
		set_rgb(
			ascii_to_byte(rgb + 1),
			ascii_to_byte(rgb + 3),
			ascii_to_byte(rgb + 5));
		uart_printstr("\r\n");
		index_str = 0;
	}
}
int main()
{
	rgb_init();
	uart_init();
	sei();
	while (1)
	{
	}
}
