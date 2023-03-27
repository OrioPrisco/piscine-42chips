#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "adc.h"
#include "i2c.h"
#include "gpio.h"
#include "pca9555.h"

unsigned int i = 0;

ISR(TIMER1_COMPA_vect)
{
	i++;
	i %= 10000;
}

int main()
{
	timer1_1second_int_init();
	uart_init();
	i2c_init();
	i2c_expander_init();
	sei();
	while (1)
	{
		_7seg_putnbr(i);
		_delay_ms(1);
	}
}
