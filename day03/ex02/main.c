#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define MYUBRR ((F_CPU/8/BAUD-1)/2)

void uart_init()
{
	//asynchronous normal mode
	//U2Xn is 0 by default
	//UMSELn too
	UBRR0 = MYUBRR;	//baud rate
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);//enable tx and rx
	UCSR0C = (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00); // frame format, 8 data 1 stop
}

void uart_tx(unsigned char c)
{
	while (!(UCSR0A & (1<<UDRE0)))
		;
	UDR0 = c;
}

unsigned char uart_rx()
{
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)))
	;
	/* Get and return received data from buffer */
	return UDR0;
}

int main()
{
	uart_init();
	while (1)
	{
		uart_tx(uart_rx());
	}
}
