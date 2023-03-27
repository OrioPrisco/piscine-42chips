#include <avr/eeprom.h>
#include "uart.h"
#include "gpio.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pca9555.h"
#include "i2c.h"

#define EEPROM_DATA_ADDR 0x0
#define EEPROM_SLECTER_ADDR ((void *)0x02)

volatile unsigned int  counters = 0xffff;
volatile unsigned char index_counters = 0;

static char get_counter()
{
	return (counters & (0xF << (4 * index_counters))) >> (4 * index_counters);
}

static void set_counter(char c)
{
	counters &= ~(0xF << (4 * index_counters));
	counters |= c   << (4 * index_counters);
}
//straight from day00_ex05
ISR(INT0_vect, ISR_BLOCK)
{
	_delay_ms(1);				// Debounce
	if (!(SWITCHES & SW1MASK))
		set_counter((get_counter() + 1)%0x10);
	eeprom_update_word(EEPROM_DATA_ADDR, counters);
	EIFR	|= (1 << INTF0);	// Clear Interrupt request flag
	//It might have been set during the delay
}

ISR(PCINT2_vect, ISR_BLOCK)
{
	_delay_ms(1);				//Debounce
	if (!(SWITCHES & SW2MASK))
		index_counters = (index_counters + 1)%4;
	eeprom_update_word(EEPROM_SLECTER_ADDR, index_counters);
	PCIFR	|= (1 << PCIF2);	// Clear interrupt request flag
	//It might have been set during the delay
}

int main()
{
	uart_init();
	i2c_init();
	i2c_expander_init();
	eeprom_busy_wait();
	counters = eeprom_read_word(EEPROM_DATA_ADDR);
	index_counters = eeprom_read_byte(EEPROM_SLECTER_ADDR)%4;
	leds_init();
	sw1_int_enable();
	sw2_int_enable();
	sei();
	while (1)
	{
		display_leds(get_counter());
		cli();
		_7seg_display_word(counters);
		sei();
		_delay_ms(1);
	}
}

