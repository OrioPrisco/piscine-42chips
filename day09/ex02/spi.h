#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

#define SPI_DDR DDRB
#define SPI_MOSI PB3
#define SPI_MISO PB4
#define SPI_SCK PB5
#define SPI_SS PB2

char spi_write(unsigned char c);
void spi_wait();
void spi_master_init();
#endif
