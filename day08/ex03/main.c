#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "adc.h"
#include "i2c.h"
#include "gpio.h"
#include "pca9555.h"
#include "spi.h"

int main()
{
	long int colors[] = {
		0x010000,
		0x000100,
		0x000001,
		0x010100,
		0x000101,
		0x010001,
		0x010101,
	};
	uart_init();
	spi_master_init();
	adc_init();
	adc_select_device(ADC_POT);
	int i = 0;
	int num = sizeof(colors)/sizeof(colors[0]);
	while(1)
	{
		unsigned char pot = adc_measure8();
		char level = (pot + 1) / (256/3);
		spi_leds_start_frame();
		for (int j = 0; j < 3; j++)
		{
			spi_leds_send(
				0b11111111,
				(level > j) * colors[(i) % num] >> 16,
				(level > j) * colors[(i) % num] >> 8,
				(level > j) * colors[(i) % num]);
		}
		spi_leds_end_frame();
		i += 1;
		i %= num;
		_delay_ms(100);
	}
}
