#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "adc.h"
#include "i2c.h"
#include "gpio.h"
#include "pca9555.h"



unsigned int i = 0;
_Bool previous_state = 0;

ISR(TIMER1_COMPA_vect)
{
	i++;
	i %= 10000;
}
int main()
{
	const unsigned int		prescaler = 256;// 1024 is too unprecise
	const double			desired_frequency = 1;
	const unsigned int		ocr1a = (F_CPU)/(prescaler * desired_frequency) - 1;
	//formula is from the manuals, except that ocr1 is expressed in term of desired frequency,
	//instead of the he frequency in term of ocr1a
	TCCR1B |= (1 << WGM12);  //Set Timer 1 to stop at OCR1A
	OCR1A = ocr1a;
	TIMSK1 = (1 << OCIE1A);
	TCCR1B |= (1 << CS12);//set clock prescaler to 256

	uart_init();
	i2c_init();
	i2c_expander_init();
	sei();
	while (1)
	{
		_7seg_putnbr(i);
		_delay_ms(1);
	}
}
