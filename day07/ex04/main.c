#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "adc.h"
#include "gpio.h"

int main()
{
	uart_init();
	adc_init();
	rgb_init();
	leds_init();
	sei();
	adc_select_device(ADC_POT);
	while (1)
	{
		unsigned char pot = adc_measure8();
		color_wheel(pot);
		unsigned char level = (pot + 1) / (256 / 4) - 1;
		unsigned char bits = (1 << (level + 1)) - 1;
		display_leds(bits);
		_delay_ms(20);
	}
}
