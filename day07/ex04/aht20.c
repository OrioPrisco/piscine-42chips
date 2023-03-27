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

void temp_sensor_get_measurment(unsigned char data[7])
{
	temp_sensor_trigger_measurment();
//	uart_printstr("asked to take a measure\r\n");
	do {
//		uart_printstr("Waiting...\r\n");
		_delay_ms(80);//spec
	} while ((temp_sensor_get_status() & TEMP_SENSOR_READY_MASK));
	i2c_start(TEMP_SENSOR_SLA, TW_READ);
	i2c_read_n(7, data, NACK);
	i2c_stop();
}

unsigned char temp_sensor_get_status()
{
	i2c_start(TEMP_SENSOR_SLA, TW_WRITE);
	unsigned char status = i2c_read_status_ack(TW_MT_DATA_ACK, NACK);
	i2c_stop();
	return status;
}
