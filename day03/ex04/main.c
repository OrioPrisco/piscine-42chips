#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define LED1 PB0
#define LED2 PB1
#define LED3 PB2
#define LED4 PB4
#define LEDS PORTB
#define LEDS_DDR DDRB
#define LED1MASK (1 << LED1)
#define LED2MASK (1 << LED2)
#define LED3MASK (1 << LED3)
#define LED4MASK (1 << LED4)

#define LEDR PD5
#define LEDG PD6
#define LEDB PD3
#define LEDRGB PORTD
#define LEDRGB_DDR DDRD
#define LEDRMASK (1 << LEDR)
#define LEDGMASK (1 << LEDG)
#define LEDBMASK (1 << LEDB)

#define SW1 PD2
#define SW2 PD4
#define SWITCHES PIND
#define SWS_DDR DDRD
#define SW1MASK (1 << SW1)
#define SW2MASK (1 << SW2)

#define CLOCK_MASK ((1 << CS00)|| ())
#define CLOCK_0    0b000
#define CLOCK_1    0b001
#define CLOCK_8    0b010
#define CLOCK_64   0b011
#define CLOCK_256  0b100
#define CLOCK_1024 0b101

#define PRESCALER				1024L	// need every litle bit of help with 8 bit timer
#define DESIRED_FREQUENCY		1L
#define STEPS					30
#define FREQUENCY_REG			((F_CPU)/((4 * PRESCALER * DESIRED_FREQUENCY * STEPS))) //formula from manual
#define BLINKING_FREQ			10000L //lower means it doesn't look like it's blinking
#define BLINKING_FREQ_STEP		(BLINKING_FREQ/STEPS)

void success()
{
	LEDS |= 0
			| LED1MASK
			| LED2MASK
			| LED3MASK
			| LED4MASK
		;
	SREG		|= (1 << 7);		// enable Interrupts

	//Timer 0
	TCCR0A	|= (1 << WGM00);					//|Mode 5 : PWM Phase correct top : OCRA
	TCCR0B	|= (1 << WGM02);					//|
	OCR0A	= FREQUENCY_REG;					// TOP
	TIMSK0	|= (1 << OCIE0A);					// Enable interrupt on Match A (Timer 0)
	TCCR0B	|= CLOCK_1024;						// set clock prescaler to 1024
	//the timer has now started !

	//Timer 1
//	TCCR1A	|= (1 << COM1A0) | (1 << COM1A1);	// toggle on upcounting and downcounting
	TCCR1B	|= (1 << WGM13);					// Mode 8, PWN Phase and frequency correct Top ICR1
	ICR1	= BLINKING_FREQ;					// TOP
	OCR1A	= 0;								// COMPARE, (0% duty cycle at the start)
	TIMSK1	|= (1 << OCIE1A);					// Enable interrupt on Match A (Timer 1)
	TCCR1B	|= CLOCK_1;							// set clock prescaler to 1, fast
	//the timer has now started !
}

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
	LEDS ^= 0
			| LED1MASK
			| LED2MASK
			| LED3MASK
			| LED4MASK
		;
}

_Bool rising = 0;
ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	if (!rising)
	{
		if (OCR1A < BLINKING_FREQ_STEP)
		{
			OCR1A = 0;
			rising = 1;
		}
		else
			OCR1A -= BLINKING_FREQ_STEP;
	}
	else
	{
		if (OCR1A > BLINKING_FREQ - BLINKING_FREQ_STEP)
		{
			OCR1A = BLINKING_FREQ;
			rising = 0;
		}
		else
			OCR1A += BLINKING_FREQ_STEP;
	}
	uart_printstr("\r\n");
	ft_putbnr(OCR1A);
}

#define MYUBRR ((F_CPU/8/BAUD-1)/2)
#define MAX_LEN 30
#define ENTER '\r'
#define BACKSPACE 0x7F //DEL
#define BEGIN_PROMPT "Enter your login:\r\n\tusername : "
#define PASSWORD_PROMPT "\r\n\tpassword : "
#define BAD_CREDENTIALS "\r\nBad combination username/password\r\n\n"
#define DEL_CHAR "\b \b"

typedef enum state {
	USERNAME = 0,
	PASSWORD = 1,
	VALIDATED = 2,
	DISPLAYING = 3,
}	e_state;

e_state	state;
short	passwd_len = 0;
short	username_len = 0;
char	buff_passwd[MAX_LEN];
char	buff_username[MAX_LEN];
char*	username = "orio";
char*	password = "hunter2";

void uart_tx(unsigned char c)
{
	while (!(UCSR0A & (1<<UDRE0)))
		;
	UDR0 = c;
}

void	ft_putbnr(int i)
{
	if (i > 9)
		ft_putbnr(i/10);
	uart_tx('0'+(i%10));
}

int	ft_strnlen(const char *s, int n)
{
	int size = 0;

	while (size < n)
	{
		if (!*s++)
			return (size);
		size++;
	}
	return (size);
}

static int	ft_memcmp(const unsigned char *s1, const unsigned char *s2, int n)
{
	while (n)
	{
		if (*s1 != *s2)
			break ;
		s1++;
		s2++;
		n--;
	}
	if (n)
		return (*s1 - *s2);
	return (0);
}

int	ft_strncmp(const char *s1, const char *s2, int n)
{
	int	len;

	if (!n)
		return (0);
	len = ft_strnlen(s1, ft_strnlen(s2, n));
	if (len < n)
		len++;
	return (ft_memcmp(s1, s2, len));
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

void uart_printstr(const char *str)
{
	while(*str)
		uart_tx(*str++);
}

void validate()
{
	if (!ft_strncmp(buff_passwd, password, passwd_len)
		&& !ft_strncmp(buff_username, username, username_len))
	{
		state = VALIDATED;
		uart_printstr("\r\nHello ");
		uart_printstr(username);
		uart_printstr(" !");
		success();
	}
	else
	{
		state = USERNAME;
		passwd_len = 0;
		username_len = 0;
		uart_printstr(BAD_CREDENTIALS);
		uart_printstr(BEGIN_PROMPT);
	}
}

void username_handler(unsigned char c)
{
	switch (c)
	{
		case BACKSPACE:
			if (username_len > 0)
			{
				--username_len;
				uart_printstr(DEL_CHAR);
			}
			break;
		case ENTER:
			uart_printstr(PASSWORD_PROMPT);
			state = PASSWORD;
			break;
		default:
			if (username_len >= MAX_LEN)
				break;
			buff_username[username_len++] = c;
			uart_tx(c);
			break;
	}
}
void password_handler(unsigned char c)
{
	switch (c)
	{
		case BACKSPACE:
			if (passwd_len > 0)
			{
				--passwd_len;
				uart_printstr(DEL_CHAR);
			}
			break;
		case ENTER:
			validate();
			break;
		default:
			if (passwd_len >= MAX_LEN)
				break;
			buff_passwd[passwd_len++] = c;
			uart_tx('*');
			break;
	}
}

ISR(USART_RX_vect, ISR_BLOCK)
{
	unsigned char c = UDR0;
	switch(state)
	{
		case USERNAME:
			username_handler(c);
			break;
		case PASSWORD:
			password_handler(c);
			break;
		default:
			break;
	}
}

int main()
{
	uart_init();
	uart_printstr(BEGIN_PROMPT);
	state = USERNAME;
	sei();
	while (1)
	{
	}
}
