#ifndef UTIL_H
#define UTIL_H

#include "stddef.h"
char nibble_to_ascii(unsigned char c);
_Bool is_hex(char c);
_Bool is_digit(char c);
//passing invalid hex ascii is ub
//uppercase or lowercase is fine
unsigned char ascii_to_nibble(char c);
unsigned char ascii_to_byte(const char *c);
unsigned char ascii_to_dec(const char *c);
short bcd_to_dec(unsigned char data);
unsigned char dec_to_bcd(unsigned short number);
size_t ft_strlen(volatile char *str);
void ft_strcpy(volatile char *dest, volatile char *src);
#endif
