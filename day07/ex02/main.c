#include <util/delay.h>
#include "uart.h"
#include "adc.h"

int main()
{
	uart_init();
	adc_init();
	uart_printstr("====\r\n");
	while (1)
	{
		adc_select_device(ADC_POT);
		uart_putnbr(adc_measure10());
		uart_printstr(", ");
		adc_select_device(ADC_LDR);
		uart_putnbr(adc_measure10());
		uart_printstr(", ");
		adc_select_device(ADC_NTC);
		uart_putnbr(adc_measure10());
		uart_printstr("         \r");//clean potential leftover characters + chariot return
		_delay_ms(20);
	}
}
