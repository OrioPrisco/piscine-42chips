#include <util/delay.h>
#include "uart.h"
#include "adc.h"

int main()
{
	uart_init();
	adc_init();
	adc_select_device(ADC_POT);
	uart_printstr("====\r\n");
	while (1)
	{
		uart_print_hex(adc_measure8());
		uart_printstr("\r");
		_delay_ms(20);
	}
}
