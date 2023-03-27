#include "pca9555.h"
#include <util/delay.h>
#include "i2c.h"
#include <util/twi.h>
#include "utils.h"
#include "uart.h"

unsigned char ascii_to_mask(char to_display)
{
	switch (to_display) {
		case '0':
		case 'O':
			return 0b00111111;
		case '1':
		case 'I':
		case 'l':
			return 0b00000110;
		case '2':
		case 'Z':
		case 'z':
			return 0b01011011;
		case '3':
			return 0b01001111;
		case '4':
			return 0b01100110;
		case '5':
			return 0b01101101;
		case '6':
			return 0b01111101;
		case '7':
			return 0b00000111;
		case '8':
			return 0b01111111;
		case '9':
			return 0b01101111;
		case '!':
			return 0b10000110;
		case 'A':
		case 'a':
			return 0b01110111;
		case 'B':
			return 0b01111111;
		case 'b':
			return 0b01111100;
		case 'C':
			return 0b00111001;
		case 'c':
			return 0b01011000;
		case 'd':
		case 'D':
			return 0b01011110;
		case 'e':
		case 'E':
			return 0b01111001;
		case 'f':
		case 'F':
			return 0b01110001;
		case 'G':
		case 'g':
			return 0b00111101;
		case 'H':
		case 'x':
		case 'X':
			return 0b01110110;
		case 'h':
			return 0b01110100;
		case 'i':
			return 0b00000100;
		case 'j':
		case 'J':
			return 0b00011110;
		case 'k':
		case 'K':
			return 0b01110101;
		case 'L':
			return 0b00111000;
		case 'M':
		case 'N':
		case 'n':
		case 'm':
			return 0b01010100;
		case 'o':
			return 0b01011100;
		case 'P':
		case 'p':
			return 0b01110011;
		case 'Q':
		case 'q':
			return 0b01100111;
		case 'R':
		case 'r':
			return 0b01010000;
		case 's':
		case 'S':
			return 0b01101101;
		case 't':
		case 'T':
			return 0b01111000;
		case 'W':
		case 'U':
		case 'V':
			return 0b00111110;
		case 'w':
		case 'u':
		case 'v':
			return 0b00011100;
		case 'Y':
		case 'y':
			return 0b01110010;
		case '.':
			return 0b10000000;
		case '-':
			return 0b01000000;
		default:
			return 0;
	}
}

void reset_7segment()
{
	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_OUTPUT1);
	i2c_write(0);
}

unsigned char _7seg_keep_mask()
{
	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_OUTPUT0);
	i2c_start(I2C_EXPANDER_SLA, TW_READ);
	unsigned char keep_mask = i2c_read_status_ack(TW_MR_DATA_ACK, ACK) & ~I2C_EXPANDER_CA_MASK;
	return keep_mask;
}

void _7seg_display_one(char segment, char mask, char keep_mask)
{
	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_OUTPUT0);
	i2c_write((~(1 << segment) & I2C_EXPANDER_CA_MASK) | keep_mask);
	i2c_write(mask);
}

void i2c_expander_init()
{
	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_CONFIG0);//DDR
	i2c_write(1 << I2C_EXPANDER_SW3);//sw3 input
	i2c_write(0);//everything else output
	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_OUTPUT0);//DDR
	i2c_write(~0);//reset everything to 0
	i2c_write(0);
	i2c_stop();
}

void _7seg_display_str(const char *word)
{
	unsigned char keep_mask = _7seg_keep_mask();
	short i;
	for (i = 0; i < 4 && word[i]; i ++)
	{
		_7seg_display_one(I2C_EXPANDER_CA1 + i, ascii_to_mask(word[i]), keep_mask);
		reset_7segment();
	}
	for (;i <4; i++)// this part is here to keep the execution time consistant
	{
		_7seg_display_one(I2C_EXPANDER_CA1 + i, 0, keep_mask);
		reset_7segment();
	}
	i2c_stop();
}

void _7seg_display_byte(unsigned char b)
{
	unsigned char keep_mask = _7seg_keep_mask();
	_7seg_display_one(I2C_EXPANDER_CA3, ascii_to_mask(nibble_to_ascii(b/16)), keep_mask);
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA4, ascii_to_mask(nibble_to_ascii(b%16)), keep_mask);
	reset_7segment();
	i2c_stop();
}

void _7seg_display_word(unsigned int b)
{
	unsigned char keep_mask = _7seg_keep_mask();
	_7seg_display_one(I2C_EXPANDER_CA4, ascii_to_mask(nibble_to_ascii(b%16)), keep_mask);
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA3, ascii_to_mask(nibble_to_ascii(b/16)), keep_mask);
	reset_7segment();
	b >>= 8;
	_7seg_display_one(I2C_EXPANDER_CA2, ascii_to_mask(nibble_to_ascii(b%16)), keep_mask);
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA1, ascii_to_mask(nibble_to_ascii(b/16)), keep_mask);
	reset_7segment();
	i2c_stop();
}

void _7seg_putnbr(int b)
{
	_7seg_putnbr_mask(b, 0);
}

void _7seg_display_time(short hours, short minutes, short seconds)
{
	_7seg_putnbr_mask(minutes + 100 * hours,
		(seconds % 2) ?
			((unsigned long int)ascii_to_mask('.') << 8)+ ((unsigned long int)ascii_to_mask('.') << 24)
			:0
	);
}

void _7seg_putnbr_one_precision(int b)
{
	if (b >= 1000 || b <= -100)
		_7seg_putnbr(b/10);
	else
		_7seg_putnbr_mask(b, (long unsigned int)ascii_to_mask('.') << 16);
}

void _7seg_putnbr_mask(int b, long unsigned int additional_masks)
{
	unsigned char keep_mask = _7seg_keep_mask();
	if (b < 0)
	{
		_7seg_display_one(I2C_EXPANDER_CA1, (additional_masks >> 0 & 0xFF) | ascii_to_mask('-'), keep_mask);
		b *= -1;
	}
	else
		_7seg_display_one(I2C_EXPANDER_CA1, (additional_masks >> 0 & 0xFF) | ascii_to_mask(nibble_to_ascii((b/1000)%10)), keep_mask);
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA2, (additional_masks >> 8 & 0xFF) | ascii_to_mask(nibble_to_ascii((b/100)%10)), keep_mask);
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA3, (additional_masks >> 16 & 0xFF) | ascii_to_mask(nibble_to_ascii((b/10)%10)), keep_mask);
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA4, (additional_masks >> 24 & 0xFF) | ascii_to_mask(nibble_to_ascii(b%10)), keep_mask);
	reset_7segment();
	i2c_stop();
}

void _7seg_display_str_delay(const char *word, int ms)
{
	
	while(ms--)
	{
		_7seg_display_str(word);
		_delay_ms(1);
	}
}

_Bool i2c_sw3_status()
{
	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_INPUT0);
	i2c_start(I2C_EXPANDER_SLA, TW_READ);
	return (i2c_read() >> I2C_EXPANDER_SW3) & 0x1;
}

void i2c_display_leds(char c)
{
	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_OUTPUT0);
	i2c_start(I2C_EXPANDER_SLA, TW_READ);

	unsigned char current = i2c_read_status_ack(TW_MR_DATA_ACK, ACK);
	current &= ~I2C_EXPANDER_LEDS_MASK;
	current |= (~c << I2C_EXPANDER_LED11) & I2C_EXPANDER_LEDS_MASK;

	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_OUTPUT0);
	i2c_write(current);
	i2c_stop();
}
