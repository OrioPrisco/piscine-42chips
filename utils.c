#include "utils.h"

char nibble_to_ascii(unsigned char c)
{
	const char *hex_alpha = "0123456789abcdef";
	return hex_alpha[c%16];
}

short bcd_to_dec(unsigned char data)
{
	return (data & 0xF) + (((data & 0xF0)>>4) * 10);
}

unsigned char dec_to_bcd(unsigned short number)
{
	return (number % 10) + (((number / 10) % 10) << 4);
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

_Bool is_digit(char c)
{
	if (c >= '0' && c <= '9')
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

unsigned char ascii_to_dec(const char *c)
{
	return ascii_to_nibble(c[1]) + (ascii_to_nibble(c[0]) * 10);
}

size_t ft_strlen(volatile char *str)
{
	size_t size = 0;
	while(*str++)
	{
		size++;
	}
	return size;
}

void ft_strcpy(volatile char *dest, volatile char *src)
{
	while (*src)
		*dest++ = *src++;
	*dest = 0;
}

int	ft_strnlen(const char *s, int n)
{
	int size = 0;

	while (size < n)
	{
		if (!*s++)
			return (size);
		size++;
	}
	return (size);
}

static int	ft_memcmp(const unsigned char *s1, const unsigned char *s2, int n)
{
	while (n)
	{
		if (*s1 != *s2)
			break ;
		s1++;
		s2++;
		n--;
	}
	if (n)
		return (*s1 - *s2);
	return (0);
}

int	ft_strncmp(const char *s1, const char *s2, int n)
{
	int	len;

	if (!n)
		return (0);
	len = ft_strnlen(s1, ft_strnlen(s2, n));
	if (len < n)
		len++;
	return (ft_memcmp((void *)s1, (void *)s2, len));

}

static size_t rotate(size_t value, size_t rotate)
{
	return (value << rotate) + (value >> (sizeof(value) * 8 - rotate));
}

size_t hash_str(const char* str, size_t length) {
	size_t i = 0;
	size_t hash = 0xDEAD;
	while (i < length) {
		hash ^= str[i++];
		hash += rotate(hash, 5);
	}
	return hash;
}
