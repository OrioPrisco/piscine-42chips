#include "uart.h"
#include "utils.h"
#include <avr/io.h>

void uart_tx(unsigned char c)
{
	while (!(UCSR0A & (1<<UDRE0)))
		;
	UDR0 = c;
}

void uart_printstr(const char *str)
{
	while (*str)
		uart_tx(*str++);
}

void uart_debugstr(const char *str)
{
	if (DEBUG)
		uart_printstr(str);
}

void uart_print_hex(unsigned char c)
{
	uart_tx(nibble_to_ascii(c/16));
	uart_tx(nibble_to_ascii(c%16));
}

void uart_putnbr(int i)
{
	if (i < 0)
	{
		uart_tx('-');
		return uart_putnbr(-i);
	}
	if (i > 9)
		uart_putnbr(i/10);
	uart_tx('0' + i%10);
}

void uart_init()
{
	//asynchronous normal mode
	//U2Xn is 0 by default
	//UMSELn too
	UBRR0 = MYUBRR;	//baud rate
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);//enable tx and rx
	UCSR0C = (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00); // frame format, 8 data 1 stop
	UCSR0B |= (1 << RXCIE0);// interupt for data receive
}
