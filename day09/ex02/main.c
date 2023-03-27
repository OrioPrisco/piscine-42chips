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
	_7seg_display_one(I2C_EXPANDER_CA4, ~(1 << I2C_EXPANDER_7SEG_f | 1 << I2C_EXPANDER_7SEG_c), _7seg_keep_mask());
	while (1)
	{
	}
}
