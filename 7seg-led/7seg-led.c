/*	Filename: 7SEG-LED.c
	Description: Application Example
	Language: HI-TECH C PRO(Lite mode)
	Target: PIC16F88, 7SEG-LED UNIT
*/

#include <pic.h>
#include "i2cs-led7.h"
#define _XTAL_FREQ 8000000

__CONFIG(
	  MCLRDIS
	& PWRTEN
	& BORDIS
	& UNPROTECT
	& WDTDIS
	& LVPDIS
	& INTIO
	& CCPRB3
	& DEBUGDIS
);

__CONFIG(
	  FCMDIS
	& IESODIS
);

__IDLOC(FF88);

void an6_init(){
	CMCON = 0b00000111;

	RBPU = 1;
	TRISB7 = 1;
	ANS6 = 1;

	ADCON0 = 0b10110001;
	ADCON1 = 0b10000000;
}

unsigned int an6_getv(void){
	__delay_us(20);

	GODONE = 1;
	while(GODONE);

	return((ADRESH << 8) | ADRESL);
}

void delay_s(unsigned char s){
	unsigned char i;

	while(s--)
		for(i = 0; i < 100; i++)
			__delay_ms(10);
}

void main(void){
	OSCCON = 0b01110000;

	led7_init();
	i2cs_init();
	PEIE = 1;
	ei();

	while(SSPIF == 0);

	while(1){
		while(STOP == 0);
		i2cs_sync();
		__delay_ms(50);
	}
}
