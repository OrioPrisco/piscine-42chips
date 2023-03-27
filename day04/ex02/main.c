#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>

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
#define ACK 1
#define NACK 0

//don't forget to bit shift the SLA adress'
#define TEMP_SENSOR_SLA 0x38 //rtfm
#define TEMP_SENSOR_READY_MASK 0x80 // bit 7
#define TEMP_SENSOR_CALIBRATE 0xbe
#define TEMP_SENSOR_CALIBRATION_MASK 0x18
#define TEMP_SENSOR_MEASURE 0xAC3300

//page 200
#define I2C_1  0b00
#define I2C_4  0b01
#define I2C_16 0b10
#define I2C_64 0b11

#ifndef DEBUG
#define DEBUG 0
#endif

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
	const char *hex_alpha = "0123456789ABCDEF";
	uart_tx(hex_alpha[c/16]);
	uart_tx(hex_alpha[c%16]);
	uart_tx(' ');
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


char status = '|';

static void next_status()
{
	switch(status) {
		case '|':
			status = '/';
			break;
		case '/':
			status = '-';
			break;
		case '-':
			status = '\\';
			break;
		case '\\':
			status = '|';
			break;
	}
}
//wait for 1 byte of 12c data to be sent
static void i2c_wait()
{
	while (!(TWCR & (1 << TWINT)))//wait for TWINT flag set
	{
		uart_tx(status);
		uart_tx('\b');
		next_status();
		_delay_ms(10);
	}
}

static void i2c_assert_status(unsigned char check)
{
	unsigned char status;

	status = TWSR & TW_STATUS_MASK;
	if (status != check)
		error(check, status);
}

static void i2c_assert_statuses(unsigned char check1, unsigned char check2)
{
	unsigned char status;

	status = TWSR & TW_STATUS_MASK;
	if (status != check1 && status != check2)
		error(check1, status);
}

//initiates i2c connection in MR or MT mode
void i2c_start(unsigned char sla_addr, unsigned char write_or_read)
{
	TWCR = (1 << TWSTA)|(1 << TWINT)|(1 << TWEN);//send start frame
	uart_debugstr("i2c_start : sent start frame\r\n");
	i2c_wait();
	uart_debugstr("i2c_start : Waited\r\n");
	i2c_assert_statuses(TW_START, TW_REP_START);
	TWDR = (sla_addr << 1) | write_or_read;	//load slave adress + r/w bit
	TWCR = (1 << TWINT) | (1<<TWEN);	//send it
	uart_debugstr("i2c_start : SLA+");
	uart_debugstr(write_or_read == TW_WRITE ? "W\r\n" : "R\r\n");
	i2c_wait();
	if (write_or_read == TW_WRITE)
		i2c_assert_status(TW_MT_SLA_ACK);
	else
		i2c_assert_status(TW_MR_SLA_ACK);
}

void i2c_write(unsigned char data)
{
	TWDR = data;						//load data
	TWCR = (1 << TWINT) | (1 << TWEN);	//send it
	i2c_wait();
	i2c_assert_status(TW_MT_DATA_ACK);
}

unsigned char i2c_read_status_ack(unsigned char status, _Bool ack)
{
	TWCR = (1 << TWINT)|(1 << TWEN)|(ack << TWEA); //ask for next data to be loaded
	i2c_wait();
	i2c_assert_status(status);
	return TWDR;
}
unsigned char i2c_read_status(unsigned char status)
{
	return i2c_read_status_ack(status, ACK);
}

unsigned char i2c_read_ack(_Bool ack)
{
	return i2c_read_status_ack(ack?TW_MR_DATA_ACK:TW_MR_DATA_NACK, ack);
}

unsigned char i2c_read()
{
	return i2c_read_status_ack(TW_MR_DATA_ACK, ACK);
}

void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	uart_debugstr("i2c_stop : stopped\r\n");
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

unsigned char temp_sensor_get_status()
{
	i2c_start(TEMP_SENSOR_SLA, TW_WRITE);
	unsigned char status = i2c_read_status_ack(TW_MT_DATA_ACK, NACK);
	i2c_stop();
	return status;
}

void temp_sensor_init()
{
	_delay_ms(100); // spec says to do so
	if (!(temp_sensor_get_status() & TEMP_SENSOR_CALIBRATION_MASK))
	{
		uart_printstr("Error ! Temparture sensor not calibrated !!!");
		while(1);
	}
	i2c_stop();
	_delay_ms(10);
}

void temp_sensor_trigger_measurment()
{
//	uart_printstr("trigger_measurment : starting\r\n");
	i2c_start(TEMP_SENSOR_SLA, TW_WRITE);
//	uart_printstr("command\r\n");
	i2c_write((TEMP_SENSOR_MEASURE >> 16) % 0x100);
//	uart_printstr("data0\r\n");
	i2c_write((TEMP_SENSOR_MEASURE >> 8 ) % 0x100);
//	uart_printstr("data1\r\n");
	i2c_write((TEMP_SENSOR_MEASURE >> 0 ) % 0x100);
//	uart_printstr("stopping\r\n");
	i2c_stop();
}

void format_temp_sensor_data(char data[7])
{
	long int humidity = data[1];
	humidity <<= 8;
	humidity |= data[2];
	humidity <<= 4;
	humidity |= data[3] >> 4;

	float humi_float = ((float)humidity) / (1L << 20) * 100;

	long int temperature = data[3] & 0x0F;
	temperature <<= 8;
	temperature |= data[4];
	temperature <<= 8;
	temperature |= data[5];
	
	float temp_float = ((float)temperature) / (1L << 20) * 200 - 50;

	uart_printstr("humidity : ");
	uart_putnbr((int)humi_float);
	uart_printstr(".");
	uart_putnbr((int)(humi_float * 10) % 10);
	uart_printstr("%\r\ntemperature : ");
	uart_putnbr((int)temp_float);
	uart_printstr(".");
	uart_putnbr((int)(humi_float * 100) % 100);
	uart_printstr("\r\n");
}

void temp_sensor_print_measurment()
{
	unsigned char data[7];
	temp_sensor_trigger_measurment();
//	uart_printstr("asked to take a measure\r\n");
	do {
//		uart_printstr("Waiting...\r\n");
		_delay_ms(80);//spec
	} while ((temp_sensor_get_status() & TEMP_SENSOR_READY_MASK));
	i2c_start(TEMP_SENSOR_SLA, TW_READ);
	for (int i = 0; i < 6; i++)
	{
		data[i] = i2c_read();
	}
	data[6] = i2c_read_ack(NACK);
	format_temp_sensor_data(data);
	for (int i = 0; i < 7; i++)
	{
		uart_print_hex(data[i]);
		uart_printstr(" ");
	}
	uart_printstr("\r\n");
	i2c_stop();
}

int main()
{
	uart_init();
	i2c_init();
	temp_sensor_init();
	uart_printstr("====\r\n");
	while (1)
	{
		temp_sensor_print_measurment();
		_delay_ms(100);
	}
}
