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
	LEDS_DDR	|= 0
					| LED1MASK
					| LED2MASK
					| LED3MASK
					| LED4MASK
				;				// Enables Leds
	//SW1 INT
	SWS_DDR		&= ~SW1MASK;	// SW1 : in
	EIMSK		|= (1 << INT0);	// enable INTerrupt 0 (SW1)
	EICRA		|= (1 << ISC00);// enable INT0(SW1) on logical change
	//SW2 INT
	SWS_DDR		&= ~SW2MASK;	// SW2 : in
	PCICR		|= (1 << PCIE2);// enable PCINT2
	PCMSK2		|= (1 << PCINT20);// set the PCINT2 Mask to accept PCINT20, (PD4/SW2)

	SREG		|= (1 << 7);	// enable Interrupts
	while (1)
	{
		display_leds(get_counter());
		cli();
		_7seg_display_word(counters);
		sei();
		_delay_ms(1);
	}
}

