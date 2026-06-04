/*	Filename: i2cm-tv.c
	Description: Test for NTSC-VIDEO UNIT
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, LCD-SDCARD UNIT
*/

#include <p24FJ64GA002.h>
#include "delay.h"
#include "i2cm.h"
#define I2C2_ADR 82

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
	CLKDIV = 0;

	uart1_init();
	puts1("Test started.\n");

	delay_s(1);

	i2c2_init();
	delay_s(1);
	puts1("I2C initialize done.\n");

	puts1("I2C start condition issue.\r\n");
	i2c2_start();

	puts1("Point slave address...");
	if(i2c2_write(I2C2_ADR << 1)){
		puts1("Slave not exist.\r\n");
		while(1);
	}
	puts1("Done.\r\n");

	puts1("Write 1 to slave...");
	if(i2c2_write(1)){
		puts1("Faild.\r\n");
		while(1);
	}
	puts1("Done.\r\n");

	puts1("Write 0 to slave...");
	if(i2c2_write(0)){
		puts1("Faild.\r\n");
		while(1);
	}
	puts1("Done.\r\n");

	puts1("Write 0 to slave...");
	if(i2c2_write(48)){
		puts1("Faild.\r\n");
		while(1);
	}
	puts1("Done.\r\n");

	puts1("Write 255 to slave...");
	if(i2c2_write(255)){
		puts1("Faild.\r\n");
		while(1);
	}
	puts1("Done.\r\n");
	puts1("Write 192 to slave...");
	if(i2c2_write(191)){
		puts1("Faild.\r\n");
		while(1);
	}
	puts1("Done.\r\n");
	puts1("I2C stop condition issue.\r\n");
	i2c2_stop();
	puts1("\r\n");
	delay_s(2);

	while(1);
}
