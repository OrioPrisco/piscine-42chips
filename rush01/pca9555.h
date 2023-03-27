#ifndef PCA955_H
#define PCA955_H

//last 3 bits can be configured
#define I2C_EXPANDER_SLA 0b0100000
#define I2C_EXPANDER_INPUT0 0
#define I2C_EXPANDER_INPUT1 1
#define I2C_EXPANDER_OUTPUT0 2
#define I2C_EXPANDER_OUTPUT1 3
#define I2C_EXPANDER_POLARITY0 4
#define I2C_EXPANDER_POLARITY1 5
#define I2C_EXPANDER_CONFIG0 6
#define I2C_EXPANDER_CONFIG1 7

#define I2C_EXPANDER_SW3 0
#define I2C_EXPANDER_LED11 1
#define I2C_EXPANDER_LED10 2
#define I2C_EXPANDER_LED9 3
#define I2C_EXPANDER_CA1 4
#define I2C_EXPANDER_CA2 5
#define I2C_EXPANDER_CA3 6
#define I2C_EXPANDER_CA4 7
#define I2C_EXPANDER_CA_MASK	0b11110000
#define I2C_EXPANDER_LEDS_MASK	0b00001110

#define I2C_EXPANDER_7SEG_a 0
#define I2C_EXPANDER_7SEG_b 1
#define I2C_EXPANDER_7SEG_c 2
#define I2C_EXPANDER_7SEG_d 3
#define I2C_EXPANDER_7SEG_e 4
#define I2C_EXPANDER_7SEG_f 5
#define I2C_EXPANDER_7SEG_g 6
#define I2C_EXPANDER_7SEG_DPX 7

//these functions call i2c_stop or do not use i2c
unsigned char ascii_to_mask(char to_display);
void i2c_expander_init();
void i2c_display_leds(char c);
void _7seg_display_str(const char *word);
void _7seg_display_str_delay(const char *word, int ms);
void _7seg_display_byte(unsigned char b);
void _7seg_display_word(unsigned int b);
void _7seg_putnbr(int b);
void _7seg_putnbr_one_precision(int b);//pass number * 10
void _7seg_putnbr_mask(int b, long unsigned int additional_masks);
void _7seg_display_time(short minutes, short hours, short seconds);

//these functions do not call i2c_stop() :
void reset_7segment();
void _7seg_display_one(char segment, char mask, char keep_mask);
_Bool i2c_sw3_status();
unsigned char _7seg_keep_mask();
#endif
