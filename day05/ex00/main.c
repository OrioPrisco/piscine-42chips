#include <avr/eeprom.h>
#include "uart.h"
#include "gpio.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pca9555.h"

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
	init_i2c_expander();
	eeprom_busy_wait();
	i = eeprom_read_byte(EEPROM_DATA_ADDR);
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
		display_leds(i);
		_7seg_display_byte(i);
		_delay_ms(1);
	}
}

