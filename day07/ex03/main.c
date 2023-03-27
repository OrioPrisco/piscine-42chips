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
		adc_select_device(ADC_TMP);
		uart_putnbr(adc_measure10() - 342);
		uart_printstr("         \r");//clean potential leftover characters + chariot return
		_delay_ms(20);
	}
}
