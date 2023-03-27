#include "aht20.h"
#include "i2c.h"
#include "uart.h"
#include <util/delay.h>

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
	i2c_start(TEMP_SENSOR_SLA, TW_WRITE);
	i2c_write((TEMP_SENSOR_MEASURE >> 16) % 0x100);
	i2c_write((TEMP_SENSOR_MEASURE >> 8 ) % 0x100);
	i2c_write((TEMP_SENSOR_MEASURE >> 0 ) % 0x100);
	i2c_stop();
}

void temp_sensor_get_measurment(unsigned char data[7])
{
	temp_sensor_trigger_measurment();
	do {
		_delay_ms(80);//spec
	} while ((temp_sensor_get_status() & TEMP_SENSOR_BUSY_MASK));
	i2c_start(TEMP_SENSOR_SLA, TW_READ);
	i2c_read_n(7, data, NACK);
	i2c_stop();
}

_Bool temp_sensor_collect_measurment_if_ready(unsigned char data[7])
{
	if ((temp_sensor_get_status() & TEMP_SENSOR_BUSY_MASK))
		return 0;
	i2c_start(TEMP_SENSOR_SLA, TW_READ);
	i2c_read_n(7, data, NACK);
	i2c_stop();
	return 1;
}
unsigned char temp_sensor_get_status()
{
	i2c_start(TEMP_SENSOR_SLA, TW_WRITE);
	unsigned char status = i2c_read_status_ack(TW_MT_DATA_ACK, NACK);
	i2c_stop();
	return status;
}

float temp_sensor_data_to_humidity(unsigned char data[7])
{
	long int humidity = data[1];
	humidity <<= 8;
	humidity |= data[2];
	humidity <<= 4;
	humidity |= data[3] >> 4;

	return (((float)humidity) / (1L << 20) * 100);
}

float temp_sensor_data_to_celcius(unsigned char data[7])
{
	long int temperature = data[3] & 0x0F;
	temperature <<= 8;
	temperature |= data[4];
	temperature <<= 8;
	temperature |= data[5];

	return (((float)temperature) / (1L << 20) * 200 - 50);
}

float temp_sensor_data_to_farenheit(unsigned char data[7])
{
	long int temperature = data[3] & 0x0F;
	temperature <<= 8;
	temperature |= data[4];
	temperature <<= 8;
	temperature |= data[5];

	return ((((float)temperature) / (1L << 20) * 200 - 50) * 1.8f + 32);
}
