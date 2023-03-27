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
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);//enable tx and rx
	UCSR0C = (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00); // frame format, 8 data 1 stop
	UCSR0B |= (1 << RXCIE0);// interupt for data receive
	sei();
}

void uart_tx(unsigned char c)
{
	while (!(UCSR0A & (1<<UDRE0)))
		;
	UDR0 = c;
}

ISR(USART_RX_vect, ISR_BLOCK)
{
	unsigned char c = UDR0;
	if (c >= 'a' && c <= 'z')
		c -= 32;
	else if (c >= 'A' && c <= 'Z')
		c += 32;
	uart_tx(c);
}

int main()
{
	uart_init();
	while (1)
	{
	}
}
