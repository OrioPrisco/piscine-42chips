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
//inits spi in master mode and resets spi leds
void spi_master_init();
//inits spi in master mode
void spi_master_reinit();
//if spi is initialiazed, turns off spi leds
//turns off spi
void spi_uninit();
void spi_leds_start_frame();
void spi_leds_end_frame();
void spi_leds_send(char global, char red, char green, char blue);
void spi_leds_send_all(char global, char red, char green, char blue);
#endif
