#ifndef PCF8563
#define PCF8563

#define RTC_SLA 0b1010001

#define RTC_CONTROL_STATUS_1 0
#define RTC_CONTROL_STATUS_2 1
#define RTC_VL_SECONDS 2
#define RTC_MINUTES 3
#define RTC_HOURS 4
#define RTC_DAYS 5
#define RTC_WEEKDAYS 7
#define RTC_CENTURY_MONTH 7
#define RTC_YEARS 8
#define RTC_MINUTE ALARM 9
#define RTC_HOUR_ALARM 10
#define RTC_DAY_ALARM 11
#define RTC_WEEKDAY_ALARM 12
#define RTC_CLKOUT_CONTROL 13
#define RTC_TIMER_CONTROL 14
#define RTC_TIMER 15

#define RTC_SECONDS_MASK	0b01111111
#define RTC_MINUTES_MASK	0b01111111
#define RTC_HOURS_MASK		0b00111111
#define RTC_DAYS_MASK		0b00111111
#define RTC_MONTH_MASK		0b00011111

//20 means 21st century
#define DEFAULT_CENTURY 2000

typedef struct {
	unsigned short seconds;
	unsigned short minutes;
	unsigned short hours;
	unsigned short day;
	unsigned short month;
	unsigned int year;
} rtc_data_t;

void rtc_raw_read(unsigned char data[15]);
void rtc_read(rtc_data_t *data);
void rtc_init();
void rtc_interpret_data(unsigned char data[15], rtc_data_t *formatted);
_Bool rtc_validate(rtc_data_t *data);
void rtc_write(rtc_data_t *data);

#endif
