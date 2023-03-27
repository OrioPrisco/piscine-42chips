#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "i2c.h"
#include "uart.h"
#include "aht20.h"

void format_temp_sensor_data(unsigned char *data)
{

	unsigned long humidity = 0;
	unsigned long temperature = 0;
	float humi_float;
	float temp_float;

	humidity = data[1];
	humidity <<= 8;
	humidity += data[2];
	humidity <<= 4;
	humidity += data[3] >> 4;

	humi_float = ((float)humidity)/(1L << 20) * 100;

	temperature = data[3] & 0x0F;
	temperature <<= 8;
	temperature += data[4];
	temperature <<= 8;
	temperature += data[5];

	temp_float = ((float)temperature)/(1L << 20)*200.0-50.0;

	uart_printstr("humidity : ");
	uart_putnbr((int)humi_float);
	uart_printstr("%\r\ntemperature : ");
	uart_putnbr((int)temp_float);
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
	} while ((temp_sensor_get_status() & TEMP_SENSOR_BUSY_MASK));
	i2c_start(TEMP_SENSOR_SLA, TW_READ);
	for (int i = 0; i < 6; i++)
	{
		data[i] = i2c_read();
	}
	data[6] = i2c_read_ack(NACK);
	format_temp_sensor_data(data);
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
