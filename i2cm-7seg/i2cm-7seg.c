/*	Filename: i2cm-7seg.c
	Description: Test for 7SEG-LED UNIT
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, LCD-SDCARD UNIT
*/

#include <p24FJ64GA002.h>
#include "delay.h"
#include "i2cm.h"
#define I2CS_ADR 83

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
	unsigned int n;

	CLKDIV = 0;
	i2c2_init();
	n = 0;

	while(1){
	delay_s(1);
	i2c2_start();
	i2c2_write(I2CS_ADR << 1);
	i2c2_write(n >> 8);
	i2c2_write(n++ & 0x00ff);
	i2c2_write(4);
	i2c2_stop();
	}
}
