#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "adc.h"
#include "i2c.h"
#include "gpio.h"
#include "pca9555.h"
#include <stddef.h>

ISR(USART_RX_vect, ISR_BLOCK)
{
	uart_rx_str_interrupt();
}

size_t ft_strlen(volatile char *str)
{
	size_t size = 0;
	while(*str++)
	{
		size++;
	}
	return size;
}

void ft_strcpy(volatile char *dest, volatile char *src)
{
	while (*src)
		*dest++ = *src++;
	*dest = 0;
}

#define LEN 60

int main()
{
	uart_init();
	i2c_init();
	i2c_expander_init();
	adc_init();
	adc_select_device(ADC_POT);
	volatile char str[LEN + 5];
	volatile char str_display[LEN];
	size_t len = 0;
	size_t i = 0;
	size_t acc = 0;
	ft_strcpy(str, "     ");
	sei();
	uart_rx_str(str_display, LEN, NONBLOCK_READ);
	while (1)
	{
		if (uart_rx_str_ready())
		{
			ft_strcpy(str + 5, str_display);
			len = ft_strlen(str_display) + 5;
			i = 0;
			uart_rx_str(str_display, LEN, NONBLOCK_READ);
		}
		_7seg_display_str_delay((char *)str + i, 1);
		acc += adc_measure8()/4;
		if (acc >= 1023)
		{
			acc -= 1023;
			i++;
		}
		if (len)
			i %= len;
		else
			i = 0;
		i %= LEN;
	}
}
