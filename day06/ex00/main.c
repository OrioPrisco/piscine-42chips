#include <util/delay.h>
#include "gpio.h"

const char colors[] = {
	LEDRMASK,
	LEDGMASK,
	LEDBMASK,
	LEDRMASK | LEDGMASK,
	LEDGMASK | LEDBMASK,
	LEDBMASK | LEDRMASK,
	LEDRMASK | LEDGMASK | LEDBMASK,
};

int main()
{
	int i = 0;
	LEDRGB_DDR = LEDRMASK | LEDGMASK | LEDBMASK;
	while (1)
	{
		LEDRGB = colors[i++];
		i %= sizeof(colors);
		_delay_ms(1000);
	}
}
