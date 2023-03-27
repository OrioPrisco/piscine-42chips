#include "eeprom.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <stddef.h>
#include "utils.h"

volatile char command[128];

static const char	*find_begin_param(const char *s, size_t n)
{
	while (n-- && *s == ' ')
		s++;
	return (s);
}

static const char	*find_end_param(const char *s, size_t n)
{
	while (n-- && *s != '"')
		s++;
	return (s);
}

static _Bool get_param(const char *string, size_t n, const char **begin, const char **end)
{
	*begin = find_begin_param(string, n);
	if (!**begin || (*begin - string == 0))
		return 1;
	*end = find_end_param(*begin + 1, n - (size_t)(*begin + 1 - *string));
	if (!(**begin == '"' && **end == '"'))
		return 1;
	if (*end - *begin == 1)
		return 1;
	return 0;
}

static void do_read(const char *data, size_t n)
{
	const char *begin, *end;
	if (get_param(data, n, &begin, &end))
		return uart_printstr("cannot parse arg1\r\n");
	size_t id = hash_str(begin + 1, end - (begin + 1));
	size_t size = eepromalloc_entry_size(id);
	if (size == 0)
		return uart_printstr("No such id\r\n");
	char string[size + 1];
	if (eepromalloc_read(id, &string, size))
		return uart_printstr("Read error\r\n");
	string[size] = 0;
	uart_printstr(string);
	uart_printstr("\r\n");
}

static void do_write(const char *data, size_t n)
{
	(void)data;
	(void)n;
	const char *begin, *end;
	if (get_param(data, n, &begin, &end))
		return uart_printstr("cannot parse arg1\r\n");
	size_t id = hash_str(begin + 1, end - (begin + 1));
	if (eepromalloc_entry_exists(id))
		return uart_printstr("entry already exists\r\n");
	if (!*end || !n)
		return uart_printstr("No second parameter\r\n");
	if (get_param(end + 1, n - 1 - (size_t)(data - end), &begin, &end))
		return uart_printstr("cannot parse arg2\r\n");
	if (eepromalloc_write(id, begin + 1, end - (begin + 1) ))
		uart_printstr("Write Error\r\n");
}

static void do_forget(const char *data, size_t n)
{
	const char *begin, *end;
	if (get_param(data, n, &begin, &end))
		return uart_printstr("cannot parse arg1\r\n");
	size_t id = hash_str(begin + 1, end - (begin + 1));
	if (eepromalloc_entry_exists(id))
	{
		if (eepromalloc_free(id))
			return uart_printstr("Free error\r\n");
	}
	else
		uart_printstr("No such id\r\n");
}

ISR(USART_RX_vect, ISR_BLOCK)
{
	uart_rx_str_interrupt();
	if (uart_rx_str_ready())
	{
		if (!ft_strncmp((char *)command, "READ ", 5))
			do_read((char *)command + 5, sizeof(command) - 5);
		else if (!ft_strncmp((char *)command, "WRITE ", 6))
			do_write((char *)command + 6, sizeof(command) - 6);
		else if (!ft_strncmp((char *)command, "FORGET ", 7))
			do_forget((char *)command + 7, sizeof(command) - 7);
		else if (!ft_strncmp((char *)command, "DUMP", 4))
			ft_print_memory(0x0, 1024);
		else
			uart_printstr("Invalid Command\r\n");
		uart_rx_str(command, sizeof(command), NONBLOCK_READ);
	}
}

int main()
{
	uart_init();
	uart_rx_str(command, sizeof(command), NONBLOCK_READ);
	sei();
	while (1)
	{

	}
}
