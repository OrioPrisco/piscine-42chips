#ifndef UTIL_H
#define UTIL_H

char nibble_to_ascii(unsigned char c);
_Bool is_hex(char c);
//passing invalid hex ascii is ub
//uppercase or lowercase is fine
unsigned char ascii_to_nibble(char c);
unsigned char ascii_to_byte(const char *c);

#endif
