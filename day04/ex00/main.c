#include <avr/io.h>
#include <util/twi.h>

//GPIO
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


//Timers0
#define TIMER_MASK ((1 << CS00)|| ())
#define TIMER_0    0b000
#define TIMER_1    0b001
#define TIMER_8    0b010
#define TIMER_64   0b011
#define TIMER_256  0b100
#define TIMER_1024 0b101

//UART
#define MYUBRR ((F_CPU/8/BAUD-1)/2)

//TWI/I2C

#define SCL_FREQ 100000L //100Khz
#define SCL_PRESCALER 1
#define MY_TWBR (((F_CPU/SCL_FREQ*SCL_PRESCALER)-16)/2) //page 180

#define TEMP_SENSOR_SLA 0x38 //rtfm

//page 200
#define I2C_1  0b00
#define I2C_4  0b01
#define I2C_16 0b10
#define I2C_64 0b11

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

void uart_print_hex(unsigned char c)
{
	const char *hex_alpha = "0123456789ABCDEF";
	uart_tx(hex_alpha[c/16]);
	uart_tx(hex_alpha[c%16]);
}

void uart_putnbr(int i)
{
	if (i > 9)
		uart_putnbr(i/10);
	uart_tx('0' + i%10);
}

void error(unsigned char expected, unsigned char got)
{
	uart_printstr("\r\nError ! Expected ");
	uart_print_hex(expected);
	uart_printstr(" But got ");
	uart_print_hex(got);
	while(1);
}

void i2c_init()
{
	TWBR = MY_TWBR;//i2c clock
}

//wait for 1 byte of 12c data to be sent
static void i2c_wait()
{
	while (!(TWCR & (1 << TWINT)))//wait for TWINT flag set
		;
}

static void i2c_assert_status(unsigned char check)
{
	unsigned char status;

	status = TWSR & TW_STATUS_MASK;
	if (status != check)
		error(check, status);
	uart_print_hex(status);
}

//todo : make assert status fct ?
void i2c_start()
{

	TWCR = (1 << TWSTA)|(1 << TWINT)|(1 << TWEN);//send start frame
	i2c_wait();
	i2c_assert_status(TW_START);
	TWDR = (TEMP_SENSOR_SLA << 1) | TW_WRITE;	//load slave adress + write bit
	TWCR = (1 << TWINT) | (1<<TWEN);	//send it
	i2c_wait();
	i2c_assert_status(TW_MT_SLA_ACK);

}

void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void uart_init()
{
	//asynchronous normal mode
	//U2Xn is 0 by default
	//UMSELn too
	UBRR0 = MYUBRR;	//baud rate
	UCSR0B |= (1 << TXEN0);//enable tx
	UCSR0C = (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00); // frame format, 8 data 1 stop
	UCSR0B |= (1 << RXCIE0);// interupt for data receive
}

int main()
{
	uart_init();
	i2c_init();
	i2c_start();
	i2c_stop();
	while (1)
	{
	}
}
