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
	long int leds[] = {
		0x000000,
		0x000001,
		0x000100,
		0x010000,
	};
	uart_init();
	spi_master_init();
	int i = 0;
	int num = sizeof(leds)/sizeof(leds[0]);
	while(1)
	{
		spi_leds_start_frame();
		for (int j = 0; j < 3; j++)
		{
			spi_leds_send(
				0b11111111,
				((i/4 % num) == 0) * leds[i % num] >> (8 * j),
				((i/4 % num) == 1) * leds[i % num] >> (8 * j),
				((i/4 % num) == 2) * leds[i % num] >> (8 * j)
			);
		}
		spi_leds_end_frame();
		i += 1;
		i %= 3 * num;
		_delay_ms(250);
	}
}
