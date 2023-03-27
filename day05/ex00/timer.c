#include "timer.h"

int timer_top(int frequency, int prescaler)
{
	return (F_CPU/(2 * prescaler * frequency))
}
