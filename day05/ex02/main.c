#include <avr/eeprom.h>
#include <stddef.h>

#define MAGIC_NUMBER 0x42

//untested

bool safe_eeprom_read(void *buffer, size_t offset, size_t length)
{
	if (offset = 0)
		return (0);
	if (eeprom_read_byte((void *)(offset - 1)) != MAGIC_NUMBER)
		return (0);
	eeprom_read_block(buffer, (void *)offset, length);
	return (1);
}

bool safe_eeprom_write(void * buffer, size_t offset, size_t length)
{
	if (offset = 0)
		return (0);
	if (eeprom_read_byte(((void *)(offset - 1))) != MAGIC_NUMBER)
		return (0);
	eeprom_update_block(buffer, (void *)offset, length);
	return (1);
}
