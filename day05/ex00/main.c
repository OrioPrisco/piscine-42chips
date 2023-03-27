#include <avr/eeprom.h>
#include "uart.h"
#include "gpio.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pca9555.h"
#include "i2c.h"

#define EEPROM_DATA_ADDR 0x0

volatile unsigned char i;

//straight from day00_ex05
ISR(INT0_vect, ISR_BLOCK)
{
	_delay_ms(2);				// Debounce
	if (!(SWITCHES & SW1MASK))
		i++;
	eeprom_update_byte(EEPROM_DATA_ADDR, i);
	EIFR	|= (1 << INTF0);	// Clear Interrupt request flag
	//It might have been set during the delay
}

ISR(PCINT2_vect, ISR_BLOCK)
{
	_delay_ms(2);				//Debounce
	if (!(SWITCHES & SW2MASK))
		i--;
	eeprom_update_byte(EEPROM_DATA_ADDR, i);
	PCIFR	|= (1 << PCIF2);	// Clear interrupt request flag
	//It might have been set during the delay
}

int main()
{
	uart_init();
	i2c_init();
	i2c_expander_init();
	eeprom_busy_wait();
	i = eeprom_read_byte(EEPROM_DATA_ADDR);
	leds_init();
	sw1_int_enable();
	sw2_int_enable();
	sei();
	while (1)
	{
		display_leds(i);
		_7seg_display_byte(i);
		_delay_ms(1);
	}
}

