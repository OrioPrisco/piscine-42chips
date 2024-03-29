#include "pcf8563.h"
#include "i2c.h"
#include "utils.h"
#include "eeprom.h"
#include "uart.h"

static unsigned int get_century()
{
	if (eepromalloc_entry_exists(EEPROM_CLOCK_CENTURY))
	{
		unsigned char century;
		if (eepromalloc_read(EEPROM_CLOCK_CENTURY, &century, sizeof(century)))
		{
			uart_debugstr("get_century : error ! Couldn't read century\r\n");
			return DEFAULT_CENTURY;
		}
		return ((unsigned int)century) * 100;
	}
	unsigned char century = DEFAULT_CENTURY/100;
	if (eepromalloc_write(EEPROM_CLOCK_CENTURY, &century, sizeof(century)))
		uart_debugstr("get_century : error ! Couldn't write century\r\n'");
	return ((unsigned int)century) * 100;
	
}

static void set_century(unsigned int year)
{
	unsigned char century = year / 100;
	if (eepromalloc_write(EEPROM_CLOCK_CENTURY, &century, sizeof(century)))
		uart_debugstr("set_century : Error ! couldn't wriite century\r\n");
}

void rtc_interpret_data(unsigned char data[15], rtc_data_t *formatted)
{
	*formatted = (rtc_data_t) {
		bcd_to_dec(data[RTC_VL_SECONDS]	& RTC_SECONDS_MASK),
		bcd_to_dec(data[RTC_MINUTES]	& RTC_MINUTES_MASK),
		bcd_to_dec(data[RTC_HOURS]		& RTC_HOURS_MASK),
		bcd_to_dec(data[RTC_DAYS]		& RTC_DAYS_MASK),
		data[RTC_CENTURY_MONTH]			& RTC_MONTH_MASK,
		bcd_to_dec(data[RTC_YEARS]) + get_century(),
	};
	if (data[RTC_CENTURY_MONTH & RTC_CENTURY_MASK])
	{
		formatted->year += 100;
	}
	set_century(formatted->year);
	//TODO handle century overflow
}

void rtc_raw_read(unsigned char data[15])
{
	i2c_start(RTC_SLA, TW_WRITE);
	i2c_write(0x00);
	i2c_start(RTC_SLA, TW_READ);
	i2c_read_n(0xf, data, NACK);
}

void rtc_read(rtc_data_t *data)
{
	unsigned char raw_data[15];
	rtc_raw_read(raw_data);
	rtc_interpret_data(raw_data, data);
}

void rtc_write(rtc_data_t *data)
{
	i2c_start(RTC_SLA, TW_WRITE);
	i2c_write(RTC_VL_SECONDS);
	i2c_write(dec_to_bcd(data->seconds));
	i2c_write(dec_to_bcd(data->minutes));
	i2c_write(dec_to_bcd(data->hours));
	i2c_write(dec_to_bcd(data->day));
	i2c_write(dec_to_bcd(0));//week days
	i2c_write(dec_to_bcd(data->month));
	i2c_write(dec_to_bcd(data->year));
	set_century(data->year);
}

void rtc_init()
{
	i2c_start(RTC_SLA, TW_WRITE);
	i2c_write(RTC_CONTROL_STATUS_1);
	i2c_write(0x00);
	i2c_write(0x00);
}

_Bool rtc_validate(rtc_data_t *data)
{
	if (data->seconds > 59)
		return 0;
	if (data->minutes > 59)
		return 0;
	if (data->hours > 23)
		return 0;
	if (data->day > 31) // check correct day depending on month ?
		return 0;
	if (data->month > 12)
		return 0;
	return 1;
}
