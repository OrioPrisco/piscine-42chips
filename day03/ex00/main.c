#include <avr/io.h>
#include <util/delay.h>

#define BAUD 115200
#define MYUBRR ((F_CPU/8/BAUD-1)/2)


void uart_init()
{
	//asynchronous normal mode
	//U2Xn is 0 by default
	//UMSELn too
	UBRR0 = MYUBRR;	//baud rate
	UCSR0B = (1 << TXEN0);//enable tx
	UCSR0C = (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00); // frame format, 8 data 1 stop

}

void uart_tx(unsigned char c)
{
	while (!(UCSR0A & (1<<UDRE0)))
		;
	UDR0 = c;
}

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
