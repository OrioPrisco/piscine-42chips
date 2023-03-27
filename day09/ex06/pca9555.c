#include "pca9555.h"
#include <util/delay.h>
#include "i2c.h"
#include <util/twi.h>
#include "utils.h"

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
		case 'H':
			return 0b01110110;
		case 'o':
			return 0b01011100;
		case 'r':
			return 0b01010000;
		case 's':
		case 'S':
			return 0b01101101;
		case 'W':
		case 'U':
			return 0b00111110;
		case '.':
			return 0b10000000;
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

// TODO : add read to preserve value of other pins
void _7seg_display_one(char segment, char mask)
{
	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_OUTPUT0);
	i2c_write(~((1 << segment)));
	i2c_write(mask);
}

void i2c_expander_init()
{
	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_CONFIG0);//DDR
	i2c_write(1 << I2C_EXPANDER_SW3);//sw3 input
	i2c_write(0);//everything else output
	i2c_stop();
}

void _7seg_display_str(const char *word)
{
	short i;
	for (i = 0; i < 4 && word[i]; i ++)
	{
		_7seg_display_one(I2C_EXPANDER_CA1 + i, ascii_to_mask(word[i]));
		reset_7segment();
	}
	for (;i <4; i++)// this part is here to keep the execution time consistant
	{
		_7seg_display_one(I2C_EXPANDER_CA1 + i, 0);
		reset_7segment();
	}
	i2c_stop();
}

void _7seg_display_byte(unsigned char b)
{
	_7seg_display_one(I2C_EXPANDER_CA3, ascii_to_mask(nibble_to_ascii(b/16)));
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA4, ascii_to_mask(nibble_to_ascii(b%16)));
	reset_7segment();
	i2c_stop();
}

void _7seg_display_word(unsigned int b)
{
	_7seg_display_one(I2C_EXPANDER_CA4, ascii_to_mask(nibble_to_ascii(b%16)));
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA3, ascii_to_mask(nibble_to_ascii(b/16)));
	reset_7segment();
	b >>= 8;
	_7seg_display_one(I2C_EXPANDER_CA2, ascii_to_mask(nibble_to_ascii(b%16)));
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA1, ascii_to_mask(nibble_to_ascii(b/16)));
	reset_7segment();
	i2c_stop();
}

void _7seg_putnbr(unsigned int b)
{
	_7seg_display_one(I2C_EXPANDER_CA4, ascii_to_mask(nibble_to_ascii(b%10)));
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA3, ascii_to_mask(nibble_to_ascii((b/10)%10)));
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA2, ascii_to_mask(nibble_to_ascii((b/100)%10)));
	reset_7segment();
	_7seg_display_one(I2C_EXPANDER_CA1, ascii_to_mask(nibble_to_ascii((b/1000)%10)));
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
	i2c_write((~c & 0b111) << I2C_EXPANDER_LED11);
	i2c_stop();
}
