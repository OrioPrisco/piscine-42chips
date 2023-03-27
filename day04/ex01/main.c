#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "uart.h"
#include "aht20.h"
#include "i2c.h"

void temp_sensor_print_measurment()
{
	temp_sensor_trigger_measurment();
//	uart_printstr("asked to take a measure\r\n");
	do {
//		uart_printstr("Waiting...\r\n");
		_delay_ms(80);//spec
	} while ((temp_sensor_get_status() & TEMP_SENSOR_BUSY_MASK));
	i2c_start(TEMP_SENSOR_SLA, TW_READ);
	for (int i = 0; i < 6; i++)
	{
		uart_print_hex(i2c_read());
		uart_tx(' ');
	}
	uart_print_hex(i2c_read_ack(NACK));
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
		uart_printstr("\r\n");
		_delay_ms(100);
	}
}
