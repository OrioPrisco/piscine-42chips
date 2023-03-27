#ifndef UART_H
#define UART_H

//UART
#define MYUBRR ((F_CPU/8/BAUD-1)/2)

#ifndef DEBUG
#define DEBUG 0
#endif

void uart_tx(unsigned char c);
void uart_printstr(const char *str);
void uart_debugstr(const char *str);
void uart_print_hex(unsigned char c);
void uart_putnbr(int i);
void uart_init();

#endif
