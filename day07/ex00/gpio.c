#include "gpio.h"

void	display_leds(unsigned char i)
{
	unsigned char leds = LEDS;
	leds &= ~(
		  (1 << LED1)
		| (1 << LED2)
		| (1 << LED3)
		| (1 << LED4)
		); // reset leds
	leds |=
		  ((i >> 0) % 2) << LED1
		| ((i >> 1) % 2) << LED2
		| ((i >> 2) % 2) << LED3
		| ((i >> 3) % 2) << LED4
	; //set leds to display i
	LEDS = leds;
}

void rgb_init()
{
	LEDRGB_DDR = LEDRMASK | LEDGMASK | LEDBMASK;
	LEDRGB &= ~(LEDRMASK | LEDGMASK | LEDBMASK);
	//TIMER0
	//MODE 1 TOP : 0xFF PWM Phase correct
	//clear when up counting
	//set when down counting
	TCCR0A = (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1);
	OCR0A = 0;
	OCR0B = 0;
	TCCR0B = 1 << CS00; // no prescaler
	//TIMER2
	//MODE 1 TOP : 0xFF PWM Phase correct
	//clear when up counting
	//set when down counting
	TCCR2A = (1 << WGM20) | (1 << COM2B1);
	OCR2B = 0;
	TCCR2B = 1 << CS20;//no prescaler
}

void rgb_uninit()
{
	//TIMER0
	TCCR0B = 0;
	TCCR0A = 0;
	OCR0A = 0;
	OCR0B = 0;
	//TIMER2
	TCCR2B = 0;
	TCCR2A = 0;
	OCR2B = 0;

	LEDRGB &= ~(LEDRMASK | LEDGMASK | LEDBMASK);
}

void set_rgb(char r, char g, char b)
{
	OCR2B = b;
	OCR0A = g;
	OCR0B = r;
}

void wheel(uint8_t pos) {
	pos = 255 - pos;
	if (pos < 85) {
		set_rgb(255 - pos * 3, 0, pos * 3);
	} else if (pos < 170) {
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	} else {
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
}
