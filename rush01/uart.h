#ifndef UART_H
#define UART_H

//UART
#define MYUBRR ((F_CPU/8/BAUD-1)/2)
#include <stddef.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#define BLOCKING_READ 1
#define NONBLOCK_READ 0

void uart_tx(unsigned char c);
void uart_printstr(const char *str);
void uart_debugstr(const char *str);
void uart_print_hex(unsigned char c);
void uart_print_n_hex(unsigned char *c, size_t n);
void uart_putnbr(int i);
void uart_init();
void uart_rx_str(volatile char * str, size_t len, _Bool blocking);
_Bool uart_rx_str_ready();

//put this in the uart_rx intrrupt if you want to use uart_rx_str
void uart_rx_str_interrupt();
#endif
