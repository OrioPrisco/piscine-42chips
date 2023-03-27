#include <util/delay.h>
#include "gpio.h"


//uses TIMER0 A/B TIMER 2 B
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

void set_rgb(char r, char g, char b)
{
	OCR2B = b;
	OCR0A = g;
	OCR0B = r;
}

#define TAU (62831L)

void	angle_to_color(long int angle)
{
	char	red;
	char	green;
	char	blue;

	red = 0;
	green = 0;
	blue = 0xff * (1.0 - ((angle - TAU * (5.0 / 6.0)) / (TAU / 6.0)));
	if (angle < TAU / 6.0 || angle > TAU * (5.0 / 6.0))
		red = 0xff;
	else if (angle < TAU / 3.0)
		red = 0xff * (1.0 - ((angle - TAU / 6.0) / (TAU / 6.0)));
	else if (angle > TAU * (2.0 / 3.0))
		red = 0xff * ((angle - TAU * (2.0 / 3.0)) / (TAU / 6.0));
	if (angle < TAU / 6.0)
		green = 0xff * (angle / (TAU / 6.0));
	else if (angle < TAU / 2.0)
		green = 0xff;
	else if (angle < TAU * (2.0 / 3.0))
		green = 0xff * (1.0 - ((angle - TAU / 2.0) / (TAU / 6.0)));
	blue = blue * (angle >= TAU / 3.0);
	if (blue != 0 && angle < TAU / 2.0)
		blue = 0xff * ((angle - TAU / 3.0) / (TAU / 6.0));
	else if (blue != 0 && angle < TAU * (5.0 / 6.0))
		blue = 0xff;
	set_rgb(red, green, blue);
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
int main()
{
	long int i = 0;
	rgb_init();
	while (1)
	{
		wheel(i++);
		_delay_ms(10);
	}
}
