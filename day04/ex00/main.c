#include <avr/io.h>
#include <util/twi.h>
#include "i2c.h"
#include "uart.h"
#include "aht20.h"

int main()
{
	uart_init();
	i2c_init();
	i2c_start(TEMP_SENSOR_SLA, TW_WRITE);
	i2c_stop();
	while (1)
	{
	}
}
