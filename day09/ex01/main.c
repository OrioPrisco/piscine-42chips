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
	unsigned char i = 0;
	_Bool current_state = i2c_sw3_status();
	_Bool previous_state = current_state;
	while (1)
	{
		i2c_display_leds(i);
		_delay_ms(3);//Debounce
		while (current_state = i2c_sw3_status(), current_state == previous_state);
		previous_state = current_state;
		if (current_state == 1)
			i++;
	}
}
