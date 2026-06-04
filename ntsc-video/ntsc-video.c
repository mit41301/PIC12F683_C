/*	Filename: ntsc-video.c
	Description: Application Example
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, NTSC-VIDEO UNIT
*/

#include <p24FJ64GA002.h>
#include "tv.h"
#include "i2cs-tv.h"
#include "rtc.h"

_CONFIG1(
	JTAGEN_OFF &
	GCP_OFF &
	GWRP_OFF &
	BKBUG_OFF &
	COE_OFF &
	ICS_PGx1 &
	FWDTEN_OFF
) 

_CONFIG2(
	IESO_OFF &
	FNOSC_FRCPLL &
	FCKSM_CSDCMD &
	OSCIOFNC_ON &
	IOL1WAY_OFF &
	I2C1SEL_PRI &
	POSCMOD_NONE
)

int main(void){
	unsigned int i;
	char c;
	char date[9] = "09/12/31";
	char time[9] = "23:59:50";

	CLKDIV = 0;

	tv_init();
	i2cs_tv_init();
	rtc_init();

	for(c = 0x21; c < 0x7f; c++) tv_putch(c);
	for(i = 0; i < 256; i += 4) tv_point(i, 16);
	for(i = 0; i < 256; i += 4) tv_point(i, 32);
	tv_line(0, 48, 255, 48);

	if(
		rtc_date_save(date) == 0 &&
		rtc_time_save(time) == 0
	) rtc_setup();

	tv_locate(0, 10);
	tv_puts(RTC_DATE_STR);
	tv_puts(RTC_WEEK_STR);
	tv_locate(0, 11);
	tv_puts(RTC_TIME_STR);

	while(1){
		if(rtc_date_update()){
			tv_locate(0, 10);
			tv_puts(RTC_DATE_STR);
			tv_puts(RTC_WEEK_STR);
		}
		if(rtc_time_update()){
			tv_locate(0, 11);
			tv_puts(RTC_TIME_STR);
		}
		i2cs_com_exe();
	}
}
