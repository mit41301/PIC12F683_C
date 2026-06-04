/*	Filename: rtc.c
	Description: RTCC API
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, NTSC-VIDEO UNIT
*/

#include <p24FJ64GA002.h>
#include <stdio.h>
#include <string.h>

typedef union
{
	struct{
		unsigned char mday;
		unsigned char mon;
		unsigned char year;
		unsigned char reserved;
	};
	unsigned short w[2];
	unsigned long l;
} RTC_DATE;

typedef union
{
	struct{
		unsigned char sec;
		unsigned char min;
		unsigned char hour;
		unsigned char weekday;
	};
	unsigned short w[2];
	unsigned long l;
} RTC_TIME;

RTC_DATE rtc_date;
RTC_TIME rtc_time;

char rtc_date_buf[9];
char rtc_time_buf[9];

char *rtc_date_str(void){
	sprintf(rtc_date_buf,
		"%02x/%02x/%02x",
		rtc_date.year,
		rtc_date.mon,
		rtc_date.mday
	);
	return(rtc_date_buf);
}

char *rtc_time_str(void){
	sprintf(rtc_time_buf,
		"%02x:%02x:%02x",
		rtc_time.hour,
		rtc_time.min,
		rtc_time.sec
	);
	return(rtc_time_buf);
}

unsigned char rtc_date_update(void){
	RTC_DATE rtc_date1, rtc_date2;

	do{
		while (RCFGCALbits.RTCSYNC);

		RCFGCALbits.RTCPTR = 3;
		rtc_date1.w[1] = RTCVAL;
		rtc_date1.w[0] = RTCVAL;

		RCFGCALbits.RTCPTR = 3;
		rtc_date2.w[1] = RTCVAL;
		rtc_date2.w[0] = RTCVAL;

	} while(rtc_date1.l != rtc_date2.l);

	if(rtc_date.l != rtc_date1.l){
		rtc_date.l = rtc_date1.l;
		return(1);
	}
	return(0);
}

unsigned char rtc_time_update(void){
	RTC_TIME rtc_time1, rtc_time2;

	do{
	while (RCFGCALbits.RTCSYNC);

	RCFGCALbits.RTCPTR = 1;
	rtc_time1.w[1] = RTCVAL;
	rtc_time1.w[0] = RTCVAL;

	RCFGCALbits.RTCPTR = 1;
	rtc_time2.w[1] = RTCVAL;
	rtc_time2.w[0] = RTCVAL;

	} while(rtc_time1.l != rtc_time2.l);

	if(rtc_time.l != rtc_time1.l){
		rtc_time.l = rtc_time1.l;
		return(1);
	}
	return(0);
}

char *rtc_week_str(void){
const char *s[] = {
	"(SUN)", "(MON)", "(TUE)", "(WED)",
	"(THU)", "(FRI)", "(SAT)"
};
	rtc_time_update(); // weekday
	return((char *)s[rtc_time.weekday]);
}

void rtc_week_save(){
	unsigned char y, m, d, w;
	const char mofs[] =
	{0,  3,  3,  6,  1,  4,  6,  2,  5,  0,  3,  5};

	y = (rtc_date.year / 16) * 10 + (rtc_date.year & 0x0f);
	m = (rtc_date.mon  / 16) * 10 + (rtc_date.mon  & 0x0f);
	d = (rtc_date.mday / 16) * 10 + (rtc_date.mday & 0x0f);

	w = (6 + y + y / 4) + mofs[m - 1] + d;
	if(((y % 4) == 0) && (m < 3)) w -= 1;
	w %= 7;

	rtc_time.weekday = w;
}

void rtc_sosc(void){
	asm("mov #OSCCON, w1");
	asm("mov.b #0x32, w0");
	asm("mov #0x46, w2");
	asm("mov #0x57, w3");
	asm("mov.b w2, [w1]");
	asm("mov.b w3, [w1]");
	asm("mov.b w0, [w1]");
}

void rtc_unlock(void){
	asm volatile("disi #5");
	asm volatile("mov #0x55, w7");
	asm volatile("mov w7, _NVMKEY");
	asm volatile("mov #0xAA, w8");
	asm volatile("mov w8, _NVMKEY");
	asm volatile("bset _RCFGCAL, #13");
	asm volatile("nop");
	asm volatile("nop");
}

void rtc_setup(void){
	rtc_unlock();
	RCFGCALbits.RTCPTR = 3;
	RTCVAL = rtc_date.w[1];
	RTCVAL = rtc_date.w[0];
	RTCVAL = rtc_time.w[1];
	RTCVAL = rtc_time.w[0];
	RCFGCALbits.RTCWREN = 0;
}

void rtc_init(void){
	rtc_sosc();
	rtc_unlock();
	RCFGCAL = 0xa800; // rtcc out disable
	ALCFGRPT = 0x0000;
	RCFGCALbits.RTCWREN = 0;

	rtc_date.year = 0x09;
	rtc_date.mon = 0x12;
	rtc_date.mday = 0x31;

	rtc_time.hour = 0x23;
	rtc_time.min = 0x59;
	rtc_time.sec = 0x50;

	rtc_week_save();

	rtc_setup();
}

unsigned char xtob(char *s)
{
	unsigned char n, t;

	n = 0;
	while(*s != 0){
	     if((*s >= '0') && (*s <= '9')) t = 48;
	else if((*s >= 'A') && (*s <= 'F')) t = 55;
	else if((*s >= 'a') && (*s <= 'f')) t = 87;
	else break;
	n = (n << 4) + *s - t;
	s++;
	}
	return n;
}

unsigned char rtc_date_save(char *s){
	char *p;

	p = strtok(s, "/:");
	if(p == NULL){
		return(1);
	} else {
		rtc_date.year = xtob(p);
	}
	p = strtok(NULL, "/:");
	if(p == NULL){
		return(1);
	} else {
		rtc_date.mon = xtob(p);
	}
	p = strtok(NULL, "/:");
	if(p == NULL){
		return(1);
	} else {
		rtc_date.mday = xtob(p);
	}
	rtc_week_save();
	return(0);
}

unsigned char rtc_time_save(char *s){
	char *p;

	p = strtok(s, "/:");
	if(p == NULL){
		return(1);
	} else {
		rtc_time.hour = xtob(p);
	}
	p = strtok(NULL, "/:");
	if(p == NULL){
		return(1);
	} else {
		rtc_time.min = xtob(p);
	}
	p = strtok(NULL, "/:");
	if(p == NULL){
		return(1);
	} else {
		rtc_time.sec = xtob(p);
	}
	return(0);
}
