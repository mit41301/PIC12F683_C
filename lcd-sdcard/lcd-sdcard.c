/*	Filename: lcd-sdcard.c
	Description: Application Example
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, LCD-SDCARD UNIT
*/

#include <p24FJ64GA002.h>
#include <stdio.h>
#include "lcd.h"
#include "rtc.h"
#include "sci.h"
#include "FSIO.h"

_CONFIG1(
	JTAGEN_OFF &
	GCP_OFF &
	GWRP_OFF &
	BKBUG_OFF &
	COE_OFF &
	ICS_PGx2 &
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
	FSFILE *pFile;
	char c;
	char date[9];
	char time[9];

	CLKDIV = 0;

	uart1_init();
	puts1("LCD-SDCARD UNIT test started");

	lcd_init();
	rtc_init();

	lcd_msg("System", "Start");

	if(!FSInit()){
		lcd_msg("SD Init", "Failed");
		while(1);
	}

	pFile = FSfopen ("RTCC.TXT", WRITE);
	if(pFile == NULL){
		lcd_msg("SD Open", "Failed");
		while(1);
	}

	FSfwrite("10/12/31\r\n", 10, 1, pFile);
	FSfwrite("23:59:50\r\n", 10, 1, pFile);
	FSfclose(pFile);

	pFile = FSfopen ("RTCC.TXT", READ);
	if(pFile == NULL){
		lcd_msg("SD Open", "Failed");
		while(1);
	}

	FSfread(date, 8, 1, pFile);date[8] = NULL;
	FSfseek(pFile, 2, SEEK_CUR);
	FSfread(time, 8, 1, pFile);time[8] = NULL;
	FSfclose(pFile);

	lcd_msg("SD Test", "Done");
	if(
		rtc_date_save(date) == 0 &&
		rtc_time_save(time) == 0
	) rtc_setup();

	lcd_locate(0, 0);
	lcd_puts(rtc_date_str());
	lcd_locate(0, 1);
	lcd_puts(rtc_time_str());

	while(1){
		if(rtc_date_update()){
			lcd_locate(0, 0);
			lcd_puts(rtc_date_str());
		}
		if(rtc_time_update()){
			lcd_locate(0, 1);
			lcd_puts(rtc_time_str());
		}
	}
}
