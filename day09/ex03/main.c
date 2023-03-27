#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "adc.h"
#include "i2c.h"
#include "gpio.h"
#include "pca9555.h"


_Bool previous_state = 0;
int main()
{
	uart_init();
	i2c_init();
	i2c_expander_init();
	char i = 0;
	while (1)
	{
		_7seg_display_one(I2C_EXPANDER_CA4, ascii_to_mask(i++ + '0'));
		i %= 10;
		_delay_ms(1000);
	}
}
