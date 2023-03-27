#ifndef EEPROM_H
#define EEPROM_H

#define INVALID_ID 0
#define CLOCK_CENTURY 1

void *ft_print_memory(void *addr, unsigned int size);
bool eepromalloc_free(size_t id);
bool eepromalloc_read(size_t id, void *buffer, size_t length);
bool eepromalloc_write(size_t id, void *buffer, size_t length);
static bool eepromalloc_entry_exists(size_t id);

#endif
