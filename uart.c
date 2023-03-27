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

void uart_print_n_hex(unsigned char *c, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		uart_print_hex(c[i]);
		uart_tx(' ');
	}
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

static volatile _Bool pressed_enter = 1;
static volatile char * uart_str;
static volatile size_t uart_str_index;
static volatile size_t uart_str_len;

void uart_rx_str_interrupt()
{
	char c = UDR0;
	if (pressed_enter)
		return;
	if (c == '\r')
	{
		uart_printstr("\r\n");
		pressed_enter = 1;
		return ;
	}
	if (c == 127)
	{
		if (uart_str_index)
		{
			uart_str_index--;
			uart_str[uart_str_index] = '\0';
			uart_printstr("\b \b");
		}
		return ;
	}
	if (uart_str_index + 1 >= uart_str_len)
		return;
	uart_str[uart_str_index] = c;
	uart_str[uart_str_index + 1] = '\0';
	uart_str_index += 1;
	uart_tx(c);
}

void uart_rx_str(volatile char * str, size_t len, _Bool blocking)
{
	uart_str = str;
	uart_str_len = len;
	str[0] = '\0';
	uart_str_index = 0;
	pressed_enter = 0;
	if (blocking)
		while(!pressed_enter);
}

_Bool uart_rx_str_ready()
{
	return pressed_enter;
}
