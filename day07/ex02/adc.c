#include "adc.h"

void adc_init()
{
	ADCSRA = 0; // make sure to preoperly reset ADC to 0
	ADMUX = (1 << REFS0) | (1 << ADLAR);	// use AVCC + AREF
											// left adjust result (we use 8 but precision)
	ADCSRA =	(1 << ADEN)					// enable ADC
				| (1 << ADPS2)				// prescaler (16)
				;
}

void adc_select_device(char address)
{
	char admux = ADMUX;
	admux &= 0xF0;
	admux |= address;
	ADMUX = admux;
}

void adc_trigger_measure()
{
	ADCSRA |= (1 << ADSC);
}

void adc_wait()
{
	while (!(ADCSRA & (1 << ADIF)));
}

char adc_measure8()
{
	adc_trigger_measure();
	adc_wait();
	return ADCH;
}

int adc_measure10()
{
	adc_trigger_measure();
	adc_wait();
	int adc;
	adc = ADCL >> 6;
	adc |= ADCH << 2;
	return adc;
}
