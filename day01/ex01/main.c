#include <avr/io.h>

int main()
{
	const unsigned int	prescaler = 256;// 1024 is too unprecise
	const double		desired_frequency = 1;//Hz
	const unsigned int	frequency_reg = (F_CPU)/(2 * prescaler * desired_frequency) - 1;
	const double		duty_cycle = 0.10;
	const unsigned int	duty_reg = (1 - duty_cycle) * frequency_reg;
	//formula is from the manuals, except that REG is expressed in term of desired frequency,
	//instead of the he frequency in term of REG

	DDRB |= (1 << PB1); // set led2 to output
	TCCR1A |= (1 << COM1A0) | (1 << COM1A1);//toggle on upcounting and downcounting
	TCCR1A |= (1 << WGM11);//|
	TCCR1B |= (1 << WGM13);//|Mode 10, PWN Phase correct Top ICR1
	ICR1 = frequency_reg;
	OCR1A = duty_reg;
	TCCR1B |= (1 << CS12);//set clock prescaler to 256
	//the clock has now started !
	while (1) {}
}
