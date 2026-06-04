#define RTC_DATE_STR rtc_date_str()
#define RTC_WEEK_STR rtc_week_str()
#define RTC_TIME_STR rtc_time_str()

char *rtc_date_str(void);
char *rtc_time_str(void);
char *rtc_week_str(void);
unsigned char rtc_date_update(void);
unsigned char rtc_time_update(void);
void rtc_setup(void);
void rtc_init(void);
unsigned char rtc_date_save(char *);
unsigned char rtc_time_save(char *);
