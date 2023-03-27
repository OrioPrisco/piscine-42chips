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

void leds_init()
{
	LEDS_DDR |= 0
		| LED1MASK
		| LED2MASK
		| LED3MASK
		| LED4MASK
	;
}

void sw1_int_enable()
{
	SWS_DDR		&= ~SW1MASK;	// SW1 : in
	EIMSK		|= (1 << INT0);	// enable INTerrupt 0 (SW1)
	EICRA		|= (1 << ISC00);// enable INT0(SW1) on logical change
}

void sw2_int_enable()
{
	SWS_DDR		&= ~SW2MASK;	// SW2 : in
	PCICR		|= (1 << PCIE2);// enable PCINT2
	PCMSK2		|= (1 << PCINT20);// set the PCINT2 Mask to accept PCINT20, (PD4/SW2)

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

void color_wheel(uint8_t pos) {
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

//compatible with rgb_init
void timer1_1second_int_init()
{
	const unsigned int		prescaler = 256;// 1024 is too unprecise
	const double			desired_frequency = 1;
	const unsigned int		ocr1a = (F_CPU)/(prescaler * desired_frequency) - 1;
	//formula is from the manuals, except that ocr1 is expressed in term of desired frequency,
	//instead of the he frequency in term of ocr1a
	TCCR1B = (1 << WGM12);  //Set Timer 1 to stop at OCR1A
	OCR1A = ocr1a;
	TIMSK1 = 1<<OCIE1A;//interrupt on compare a
	TCCR1B |= (1 << CS12);//set clock prescaler to 256
	//the clock has now started !
}
