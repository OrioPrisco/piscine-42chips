#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "adc.h"
#include "i2c.h"
#include "gpio.h"
#include "pca9555.h"

int main()
{
	uart_init();
	i2c_init();
	i2c_expander_init();
	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_OUTPUT0);
	while (1)
	{
		i2c_write(~(1 << I2C_EXPANDER_LED9));
		i2c_write(0);
		_delay_ms(1000);
		i2c_write(~0);
		i2c_write(0);
		_delay_ms(1000);
	}
}
