#include <util/delay.h>
#include "gpio.h"
#include "spi.h"

int main()
{
	long int i = 0;
	rgb_init();
	while (1)
	{
		color_wheel(i++);
		_delay_ms(10);
	}
}
