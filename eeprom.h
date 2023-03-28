#ifndef EEPROM_H
#define EEPROM_H

#define EEPROM_INVALID_ID 0
#define EEPROM_CLOCK_CENTURY 1

typedef _Bool bool;
#include <stddef.h>

void *ft_print_memory(void *addr, unsigned int size);
bool eepromalloc_free(size_t id);
bool eepromalloc_read(size_t id, void *buffer, size_t length);
bool eepromalloc_write(size_t id, const void *buffer, size_t length);
bool eepromalloc_entry_exists(size_t id);
//0 means entry does not exist
size_t eepromalloc_entry_size(size_t id);

#endif
