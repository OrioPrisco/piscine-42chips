#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 115200L
#define MYUBRR ((F_CPU/8/BAUD-1)/2)

#define CLOCK_MASK ((1 << CS00)|| ())
#define CLOCK_0    0b000
#define CLOCK_1    0b001
#define CLOCK_8    0b010
#define CLOCK_64   0b011
#define CLOCK_256  0b100
#define CLOCK_1024 0b101


void uart_init()
{
	//asynchronous normal mode
	//U2Xn is 0 by default
	//UMSELn too
	UBRR0 = MYUBRR;	//baud rate
	UCSR0B = (1 << TXEN0);//enable tx
	UCSR0C = (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00); // frame format, 8 data 1 stop

}

void init_timer1()
{
	const unsigned int			prescaler = 1024;
	const double				desired_frequency = 0.5;//Hz
	const unsigned long int		ocr1a = (F_CPU)/(desired_frequency * prescaler);
	//formula is from the manual
	TCCR1B |= (1 << WGM12);  //Mode 4 CTC top OCR1A
	OCR1A = ocr1a;
	TIFR1 = (1 << OCF1A);//  |enable intterupt on OCR1A match
	TIMSK1 = (1 << OCIE1A);//|
	sei();
	TCCR1B |= CLOCK_1024;
	//clock has started
}
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

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
	uart_printstr("Hello World !\r\n");
}

int main()
{
	uart_init();
	init_timer1();
	
	while (1)
	{
	}
}
