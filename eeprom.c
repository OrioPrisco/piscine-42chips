#include <avr/eeprom.h>
#include <stddef.h>
#include "uart.h"
#include <util/delay.h>

//TODO : disable interrupts while doing anything
// if 2 thngs were to attempt a combination of read write alloca or free at the same time
// it would cause issues

#define MAX_ENTRY 0xFF
#define MAGIC_BYTE 0x42
#define EEPROM_END 1024
#define ENTRY_SIZE 6
#define ENTRIES_START 2
// top of memory belongs to malloc

//first byte : magic
//first byte : number of entries

//number * sizeof(entry) next bytes : entries

//entry :	id			: 2 byte (size_t)
//			mem_addr	: 2 byte (size_t)
//			length		: 2 byte (size_t)

//id 00 means free entry
//it is never a valid id

//TODO : make partial read
//atempting to read something else than entry.length : ERROR
//TODO : make partial write
//atempting to write something else than entry.length : ERROR
//if you attempt to read an unexisting id : ERROR

//if you attempt to write an unexisting id : attempt to find a free block
//if a free block is found, an entry will be created and written to
//it will then be valid to read from

//freeing an id will zero out the entry id
//TODO : if the last n entries are zero'd out, shrink number of entries
//TODO : if more than half? entries are zeor'd out, move entries and shrink number of entries


//TODO : add uartdebugstr

//TODO : since eeprom is 1k, only need 10 bits for a size_t
//		 would save 1 byte + 4bits
//		 this would make the struct no longer word aligned, unless i use that byte

typedef _Bool bool;

typedef struct s_entry {
	size_t id;			//id, this is how users address buffers
	size_t address;		// beginning of storage buffer
	size_t length;		// length of storage buffer
	size_t entry_addr;	// address of this struct on eeprom, not stored on eeprom
} t_entry;

static void panic_if_magic_corrupted()
{
	if (eeprom_read_byte(0x0) == MAGIC_BYTE)
		return ;
	uart_printstr("Eeprom memory corrupted !!!\r\n");
	while (1);
}

static size_t get_word(size_t address)
{
	size_t temp;

	temp = eeprom_read_byte((void *)address);
	temp <<= 8;
	temp |= eeprom_read_byte((void *)(address + 1));
	return temp;
}

static void update_word(size_t address, size_t word)
{
	eeprom_update_byte((void *)address, word >> 8);
	eeprom_update_byte((void *)(address + 1), word & 0xFF);
}

static size_t get_entry_address_from_offset(size_t offset)
{
	return ENTRIES_START + (offset * ENTRY_SIZE);
}

//fetch the nth entry, which is not necessarily the entry with id n
//does not check bounds
static void load_entry_at_offset(size_t offset, t_entry *entry)
{
	offset = ENTRIES_START + (offset * ENTRY_SIZE);
	entry->id = get_word(offset);
	entry->address = get_word(offset + 2);
	entry->length = get_word(offset + 4);
	entry->entry_addr = offset;
}

static bool eepromalloc_get_entry(size_t id, t_entry *entry)
{
	unsigned char entries = eeprom_read_byte((void *)0x1);
	for (unsigned char i = 0; i < entries; i++)
	{
		t_entry temp;
		load_entry_at_offset(i, &temp);
		if (id == temp.id)
		{
			*entry = temp;
			return (0);
		}
	}
	return (1);
}

static void update_entry(t_entry *entry)
{
	uart_debugstr("update_entry : ");
	if (DEBUG)
		uart_print_n_hex((unsigned char *)entry, sizeof(*entry));
	uart_debugstr("\r\n");
	size_t address = entry->entry_addr;
	if (address % 2)
	{
		uart_printstr("Error ! Unaligned entry, id : ");
		uart_putnbr(entry->id);
		while(1);
	}
	update_word(address, entry->id);
	update_word(address + 2, entry->address);
	update_word(address + 4, entry->length);
}

bool eepromalloc_entry_exists(size_t id)
{
	panic_if_magic_corrupted();
	t_entry entry;
	if (eepromalloc_get_entry(id, &entry))
		return (0);
	return (1);
}

size_t eepromalloc_entry_size(size_t id)
{
	panic_if_magic_corrupted();
	t_entry entry;
	if (eepromalloc_get_entry(id, &entry))
		return (0);
	return (entry.length);
}

static t_entry get_entry_with_lowest_address()
{
	unsigned char entries = eeprom_read_byte((void *)0x1);
	t_entry best = { .address = EEPROM_END, .length = 0};
	t_entry temp;
	for (unsigned char i = 0; i < entries; i++)
	{
		load_entry_at_offset(i, &temp);
		if (temp.address < best.address)
			best = temp;
	}
	return best;
}

static bool get_entry_intersecting(size_t address, size_t length, t_entry *entry)
{
	unsigned char entries = eeprom_read_byte((void *)0x1);
	t_entry temp;
	for (unsigned char i = 0; i < entries; i++)
	{
		load_entry_at_offset(i, &temp);
		// bound check
		if ((temp.address >= address && temp.address <= address + length)
			|| (temp.address + temp.length >= address && temp.address + temp.length < address + length))
		{
			*entry = temp;
			return (1);
		}
	}
	return (0);
}

//finds a contiguous block of memory between lowest and highest of size length
// address 0 means failure to find a block
// will start looking at the end and go towrad 0
static size_t find_contiguous_block(size_t end, size_t start, size_t length)
{
	size_t current = end;
	t_entry intersecting;
	while ( (current > length) && (current - length > start) && //still in valid bound
		get_entry_intersecting(current - length, length, &intersecting))
	{
		current = intersecting.address - 1;
	}
	if ((current < length) || (current - length < start))
		return 0;//failure
	return current - length;
}

//doesn't check that an already existing id exists
//TODO : check for entries with an id of 0 and a size identical to length with valid memory pointer (avoid 2 size_t writes)
//		otherwise check for entries with either a valid memory pointer or an equal length (saves 1 size_t write)
static bool eepromalloc_allocate_entry(size_t id, t_entry *entry, size_t length)
{
	uart_debugstr("going to allocate\r\n");
	t_entry free_entry;
	if (length >= EEPROM_END || length == 0)
		return 1;//can't allocate more memory than the eeprom has
				//and allocating 0 bytes is silly
	unsigned char entries = eeprom_read_byte((void *)0x1);
	if (eepromalloc_get_entry(0, &free_entry)) // find free entry
	{
		uart_debugstr("no freebies\r\n");
		if (entries == MAX_ENTRY)
			return (1);
		t_entry lowest = get_entry_with_lowest_address();
		uart_debugstr("got lowest entry\r\n");
		size_t new_entry_entry_end = get_entry_address_from_offset(entries + 1) + ENTRY_SIZE;
		if (lowest.address < new_entry_entry_end)
			return (1);//TODO : attempt to shuffle memory around to make space ?
		entries += 1;
		eeprom_update_byte((void *)0x1, entries);
		free_entry.entry_addr = get_entry_address_from_offset(entries - 1);
	}
	size_t entries_end = get_entry_address_from_offset(entries) + ENTRY_SIZE;
	uart_debugstr("let's find a block\r\n");
	size_t address = find_contiguous_block(EEPROM_END, entries_end, length);
	uart_debugstr("found an entry or made one\r\n");
	if (address == 0)
		return 1;//failure
	free_entry.length = length;
	free_entry.address = address;
	free_entry.id = id;
	update_entry(&free_entry);
	*entry = free_entry;
	return (0);
}

bool eepromalloc_write(size_t id, const void *buffer, size_t length)
{
	if (id == 0)
		return (1);
	panic_if_magic_corrupted();
	t_entry entry;
	if (eepromalloc_get_entry(id, &entry))
	{
		if (eepromalloc_allocate_entry(id, &entry, length))
			return (1);
	}
	if (length != entry.length)
		return (1);
	eeprom_update_block(buffer,(void *)entry.address, length);
	return (0);
}

bool eepromalloc_read(size_t id, void *buffer, size_t length)
{
	if (id == 0)
		return (1);
	panic_if_magic_corrupted();
	t_entry entry;
	if (eepromalloc_get_entry(id, &entry))
		return (1);
	if (length != entry.length)
		return (1);
	eeprom_read_block(buffer, (void *)entry.address, length);
	return (0);
}

bool eepromalloc_free(size_t id)
{
	if (id == 0)
		return (1);
	panic_if_magic_corrupted();
	t_entry entry;
	if (eepromalloc_get_entry(id, &entry))
		return (1);
	size_t zero = 0;
	eeprom_update_block(&zero, (void *)entry.entry_addr, 2); // zero out entry id
	return (0);
}

//==========print memory
static int	ft_char_is_printable(char c)
{
	if (c <= 31 || c >= 127)
		return (0);
	return (1);
}

static void	ft_print_address(void *addr)
{
	intptr_t		a;
	size_t			i;

	i = 0;
	a = (intptr_t)addr;
	while (i < sizeof(addr))
	{
		uart_print_hex(a >> (8* (sizeof(addr) - i - 1)));
		i++;
	}
	uart_printstr(": ");
}

static void	ft_print_hex(void *addr, unsigned int size)
{
	unsigned char	*byte;
	int				i;

	byte = addr;
	i = 0;
	while (i++ < 16)
	{
		if (size)
		{
			uart_print_hex(eeprom_read_byte((void *)byte));
			byte++;
			size--;
		}
		else
		{
			uart_printstr("  ");
		}
		if (!(i % 2))
		{
			uart_printstr(" ");
		}
	}	
}

static void	ft_print_ascii(void *addr, unsigned int size)
{
	char	*c;

	if (size > 16)
		size = 16;
	c = addr;
	while (size--)
	{
		if (ft_char_is_printable(eeprom_read_byte((void *)c)))
			uart_tx(eeprom_read_byte((void *)c));
		else
			uart_printstr(".");
		c++;
	}
	uart_printstr("\r\n");
}

void	*ft_print_memory(void *addr, unsigned int size)
{
	char	*ptr;

	if (size == 0)
		return (addr);
	ptr = addr;
	while (1)
	{
		ft_print_address(ptr);
		ft_print_hex(ptr, size);
		ft_print_ascii(ptr, size);
		ptr += 16;
		if (size < 16)
			break ;
		size -= 16;
	}
	return (addr);
}
