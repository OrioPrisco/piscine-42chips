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
	while (1)
	{
		_7seg_display_byte(0x42);
	}
}
