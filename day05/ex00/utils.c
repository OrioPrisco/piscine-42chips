#include "utils.h"

char nibble_to_ascii(unsigned char c)
{
	const char *hex_alpha = "0123456789abcdef";
	return hex_alpha[c%16];
}
