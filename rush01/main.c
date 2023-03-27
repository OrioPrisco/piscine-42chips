#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "adc.h"
#include "i2c.h"
#include "gpio.h"
#include "pca9555.h"
#include <stddef.h>
#include "spi.h"
#include "aht20.h"

// TODO : RTC
// disable SPI when not in mode 4 so led can shine bright

volatile char leds = 0;
volatile short mode = 0;

ISR(USART_RX_vect, ISR_BLOCK)
{
	uart_rx_str_interrupt();
}

//sw1
ISR(INT0_vect, ISR_BLOCK)
{
	_delay_ms(2); // debounce
	leds &= ~0b100;
	leds |= 0b100 * !(SWITCHES & SW1MASK);
	if (!(SWITCHES & SW1MASK))
		mode = (mode + 1) % 12;
	EIFR |= 1 << INT0; // reset interrupt flag
}

//sw2
ISR(PCINT2_vect, ISR_BLOCK)
{
	_delay_ms(2); // debounce
	leds &= ~0b010;
	leds |= 0b010 * !(SWITCHES & SW2MASK);
	if (!(SWITCHES & SW2MASK))
	{
		if (!mode)
			mode = 11;
		else
			mode--;
	}
	PCICR |= 1 << PCIE2; // reset interrupt flag
}

void sw3_do_things()
{
	leds &= ~0b001;
	leds |= 0b001 * !(i2c_sw3_status());
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

void mode0()
{
	adc_select_device(ADC_POT);
	_7seg_putnbr(adc_measure10());
}

void mode1()
{
	adc_select_device(ADC_LDR);
	_7seg_putnbr(adc_measure10());
}

void mode2()
{
	adc_select_device(ADC_NTC);
	_7seg_putnbr(adc_measure10());
}

void mode3()
{
	adc_select_device(ADC_TMP);
	cli();
	_7seg_putnbr(adc_measure10() - 342);
	sei();
}

static const long unsigned int colors[] = {
	0x010000,
	0x000100,
	0x000001,
};
volatile short color_index = 0;

unsigned char temp_sensor_data[7];

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
	color_index = (color_index + 1) % (sizeof(colors)/ sizeof(colors[0]));
	if (temp_sensor_collect_measurment_if_ready(temp_sensor_data))
		temp_sensor_trigger_measurment();
}

void mode4()
{
	short i = color_index;
	_7seg_display_str("-42-");
	set_rgb(
		(colors[i] >> 16) << 4,
		(colors[i] >> 8 ) << 4,
		(colors[i] >> 0 ) << 4
	);
	spi_leds_send_all(0xff,
		colors[i] >> 16,
		colors[i] >> 8,
		colors[i] >> 0
	);
	LEDS_DDR |= LED4MASK;
}

void mode5()
{
	_7seg_putnbr_one_precision((int)(temp_sensor_data_to_celcius(temp_sensor_data) * 10));
}

void mode6()
{
	_7seg_putnbr_one_precision((int)(temp_sensor_data_to_farenheit(temp_sensor_data) * 10));
}

void mode7()
{
	_7seg_putnbr_one_precision((int)(temp_sensor_data_to_humidity(temp_sensor_data) * 10));
}

void mode8()
{

}

void mode9()
{

}

void mode10()
{

}

void mode11()
{

}

void global_stuff()
{
	cli();
	sw3_do_things();
	i2c_display_leds(leds);
	sei();
	if (!(mode & 0b1000))
	{
		set_rgb(0,0,0);
		spi_leds_send_all(0xFF,0,0,0);
		LEDS_DDR |= LED4MASK;
	}
	display_leds(mode);
}

#define LEN 60

void (*modes[])(void) = {
	mode0,
	mode1,
	mode2,
	mode3,
	mode4,
	mode5,
	mode6,
	mode7,
	mode8,
	mode9,
	mode10,
	mode11,
};

int main()
{
	uart_init();
	i2c_init();
	i2c_expander_init();
	sw1_int_enable();
	sw2_int_enable();
	adc_init();
	rgb_init();
	spi_master_init();
	leds_init();
	temp_sensor_init();
	temp_sensor_trigger_measurment();
	timer1_1second_int_init();
	volatile char input[LEN];
	sei();
	uart_rx_str(input, LEN, NONBLOCK_READ);
	i2c_display_leds(0b111);
	while (1)
	{
		global_stuff();
		cli();
		modes[mode]();
		sei();
		_delay_ms(1);
	}
}
