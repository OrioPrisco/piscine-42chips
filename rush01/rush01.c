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
#include "pcf8563.h"
#include "utils.h"

#define LEN 60
volatile char leds = 0;
volatile char input[LEN];
volatile short mode = 0;
volatile unsigned short begin = 0;

void uart_parse_string()
{
	if (ft_strlen(input) != 19)
		return uart_printstr("Wrong number of characters\r\n");
	if (!(1
		&& is_digit((char)input[0])
		&& is_digit((char)input[1])
		&& (char)input[2] == '/'
		&& is_digit((char)input[3])
		&& is_digit((char)input[4])
		&& (char)input[2] == '/'
		&& is_digit((char)input[6])
		&& is_digit((char)input[7])
		&& is_digit((char)input[8])
		&& is_digit((char)input[9])
		&& (char)input[10] == ' '
		&& is_digit((char)input[11])
		&& is_digit((char)input[12])
		&& (char)input[13] == ':'
		&& is_digit((char)input[14])
		&& is_digit((char)input[15])
		&& (char)input[16] == ':'
		&& is_digit((char)input[17])
		&& is_digit((char)input[18])
	))
		return uart_printstr("Wrong format (dd/MM/YYYY HH:mm:ss)\r\n");
	rtc_data_t data;
	data.day = ascii_to_dec((char *)input);
	data.month = ascii_to_dec((char *)input + 3);
	data.year = ascii_to_dec((char *)input + 6) * 100 + ascii_to_dec((char *)input + 8);
	data.hours = ascii_to_dec((char *)input + 11);
	data.minutes = ascii_to_dec((char *)input + 14);
	data.seconds = ascii_to_dec((char *)input + 17);
	if (!rtc_validate(&data))
		return uart_printstr("Wrong value somewhere\r\n");
	rtc_write(&data);
	uart_printstr("Date written.\r\n");
}

ISR(USART_RX_vect, ISR_BLOCK)
{
	uart_rx_str_interrupt();
	if (uart_rx_str_ready())
	{
		uart_parse_string();
		uart_rx_str(input, LEN, NONBLOCK_READ);
	}
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
	if (begin <= 4)
		begin++;
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
	//do whatever
}

void mode6()
{
	_7seg_putnbr_one_precision((int)(temp_sensor_data_to_celcius(temp_sensor_data) * 10));
}

void mode7()
{
	_7seg_putnbr_one_precision((int)(temp_sensor_data_to_farenheit(temp_sensor_data) * 10));
}

void mode8()
{
	_7seg_putnbr_one_precision((int)(temp_sensor_data_to_humidity(temp_sensor_data) * 10));
}

void mode9()
{
	rtc_data_t rtc_data;
	rtc_read(&rtc_data);
	_7seg_display_time(rtc_data.hours, rtc_data.minutes, rtc_data.seconds);
}

void mode10()
{
	rtc_data_t rtc_data;
	rtc_read(&rtc_data);
	_7seg_putnbr(rtc_data.month +  rtc_data.day * 100);
}

void mode11()
{
	rtc_data_t rtc_data;
	rtc_read(&rtc_data);
	_7seg_putnbr(rtc_data.year);
}

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

void global_stuff()
{
	sw3_do_things();
	i2c_display_leds(leds);
	if (begin == 3)
	{
		display_leds(0b0);
		_7seg_display_str("    ");
	}
	if (begin >= 4)
	{
		if (mode != 4)
		{
			set_rgb(0,0,0);
			spi_uninit();
			LEDS_DDR |= LED4MASK;
		}
		else
			spi_master_reinit();
		display_leds(mode);
		modes[mode]();
	}
}

int main()
{
	uart_init();
	i2c_init();
	rtc_init();
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
	uart_rx_str(input, LEN, NONBLOCK_READ);
	spi_uninit();
	//=====================
	i2c_start(I2C_EXPANDER_SLA, TW_WRITE);
	i2c_write(I2C_EXPANDER_OUTPUT0);
	i2c_write(
		~(char)
		 ((1 << I2C_EXPANDER_CA1)
		| (1 << I2C_EXPANDER_CA2)
		| (1 << I2C_EXPANDER_CA3)
		| (1 << I2C_EXPANDER_CA4)));
	i2c_write(0b11111111);
	i2c_stop();
	display_leds(0b1111);
	//======================
	while (1)
	{
		cli();
		global_stuff();
		sei();
		_delay_ms(1);
	}
}
