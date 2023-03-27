#include "i2c.h"
#include "uart.h"
#include <util/delay.h>
#include <util/twi.h>

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

void i2c_wait()
{
	while (!(TWCR & (1 << TWINT)))//wait for TWINT flag set
		;
}

void i2c_assert_status(unsigned char check)
{
	unsigned char status;

	status = TWSR & TW_STATUS_MASK;
	if (status != check)
		error(check, status);
}

void i2c_assert_statuses(unsigned char check1, unsigned char check2)
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

//assumes MR mode
unsigned char i2c_read_n(int bytes, unsigned char *data, _Bool ack_at_end)
{
	for (int i = 0;i < bytes - 1; i++)
	{
		data[i] = i2c_read();
	}
	data[bytes - 1] = i2c_read_ack(ack_at_end);
}

//assumes MR mode
unsigned char i2c_read_ack(_Bool ack)
{
	return i2c_read_status_ack(ack?TW_MR_DATA_ACK:TW_MR_DATA_NACK, ack);
}

//assumes MR mode
unsigned char i2c_read()
{
	return i2c_read_ack(ACK);
}

void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	uart_debugstr("i2c_stop : stopped\r\n");
}
