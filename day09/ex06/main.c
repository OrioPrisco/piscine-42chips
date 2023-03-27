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
	adc_init();
	adc_select_device(ADC_POT);
	sei();
	while (1)
	{
		_7seg_putnbr(adc_measure10());
		_delay_ms(1);
	}
}
