#include "utils.h"

char nibble_to_ascii(unsigned char c)
{
	const char *hex_alpha = "0123456789abcdef";
	return hex_alpha[c%16];
}


_Bool is_hex(char c)
{
	if (c >= '0' && c <= '9')
		return 1;
	if (c >= 'A' && c <= 'F')
		return 1;
	if (c >= 'a' && c <= 'f')
		return 1;
	return 0;
}

unsigned char ascii_to_nibble(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return 0;
}

unsigned char ascii_to_byte(const char *c)
{
	return ascii_to_nibble(c[1]) + (ascii_to_nibble(c[0]) << 4);
}
