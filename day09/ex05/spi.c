#include "spi.h"
#include "uart.h"

void spi_master_init()
{
	SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);
	SPI_DDR &= ~(1 << SPI_MISO);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (1 << SPR1);
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
	volatile char dummy = SPSR;
	SPDR = c;
	spi_wait();
	spi_wait();
	return 0;
}
