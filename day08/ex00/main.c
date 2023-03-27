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
	uart_init();
	spi_master_init();
	for (int i =0; i < 4; i++)
		spi_write(~3);// 3 = second, ~3 = first
	uart_printstr("foo");
	spi_leds_start_frame();
	spi_leds_send(0b00011111, 1, 0, 0);
	spi_leds_send(0, 1, 1, 1);
	spi_leds_send(0, 1, 1, 1);
	spi_leds_end_frame();
	while (1)
	{
	}
}
