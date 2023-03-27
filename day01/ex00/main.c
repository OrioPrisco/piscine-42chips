#include <avr/io.h>

int main()
{
	const unsigned int		prescaler = 256;// 1024 is too unprecise
	const double			desired_frequency = 1;
	const unsigned int		ocr1a = (F_CPU)/(2 * prescaler * desired_frequency) - 1;
	//formula is from the manuals, except that ocr1 is expressed in term of desired frequency,
	//instead of the he frequency in term of ocr1a
	DDRB |= (1 << PB1); // set led2 to output
	TCCR1A |= (1 << COM1A0); // set Timer1 to toggle on compare
	TCCR1B |= (1 << WGM12);  //Set Timer 1 to stop at OCR1A
	OCR1A = ocr1a;
	TCCR1B |= (1 << CS12);//set clock prescaler to 256
	//the clock has now started !
	while (1)
	{

	}
}
