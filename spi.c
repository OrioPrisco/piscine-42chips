#include "spi.h"
#include "uart.h"

void spi_master_init()
{
	spi_master_reinit();
	spi_leds_send_all(0xFF,0,0,0);
}

void spi_master_reinit()
{
	SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (1 << SPR1);
}

void spi_uninit()
{
	if ((SPCR & ((1 << MSTR) | (1 << SPE))) == ((1 << MSTR) | (1 << SPE)))
		spi_leds_send_all(0xFF,0,0,0);
	SPCR = 0;
}

void spi_wait()
{
	while (!(SPSR & (1 << SPIF)))
	{
		if (SPSR & (1 << WCOL))
		{
			uart_printstr("Error : collision is spi_wait !\r\n");
			DDRB = 1 << PB0;
			PORTB = 1 << PB0;
			while (1);
		}
	}
}

char spi_write(unsigned char c)
{
	volatile char spsr = SPSR;
	SPDR = c;
	spi_wait();
	spi_wait();
	return spsr;
}

void spi_leds_start_frame()
{
	for (int i =0; i < 4; i++)
		spi_write(0);
}

void spi_leds_end_frame()
{
	for (int i =0; i < 3; i++)
		spi_write(~0);
}

void spi_leds_send(char global, char red, char green, char blue)
{
	spi_write(global);
	spi_write(blue);
	spi_write(green);
	spi_write(red);
}

void spi_leds_send_all(char global, char red, char green, char blue)
{
	spi_leds_start_frame();
	for (int i = 0; i < 3; i++)
		spi_leds_send(global, red, green, blue);
	spi_leds_end_frame();
}
