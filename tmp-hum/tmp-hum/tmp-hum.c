/*	Filename: tmp-hum.c
	Description: Application Example
	Language: HI-TECH C PRO(Lite mode)
	Target: PIC12F683, TMP-HUM UNIT
*/

#include <pic.h>
#include "scif.h"
#include <stdio.h>

#define _XTAL_FREQ 8000000

__CONFIG(
	  MCLRDIS
	& PWRTEN
	& BORDIS
	& UNPROTECT
	& WDTDIS
	& INTIO
	& FCMDIS
	& IESODIS
);

__IDLOC(F683);

void __delay_s(unsigned char s){
	unsigned char i;

	while(s--)
		for(i = 0; i < 100; i++) __delay_ms(10);
}

unsigned int bresenham(
	unsigned int n, unsigned int m, unsigned int d
){
	unsigned int e;
	unsigned int s;

	s = e = 0;
	while(n--){
		e += m;
		while(e >= d){
			e -= d;
			s++;
		}
	}
	return s;
} 

void main(void){
	unsigned int vt;
	unsigned int vh;

	// System initialize
	OSCCON = 0b01110000;
	TRISIO = 0b00011001;
	CMCON0 = 0b00000111;

	// AD converter initialize
	ANSEL = 0b00101001;
	ADCON0 = 0b10000001;

	// CCP initialize for PWM
	PR2 = 127; // 1kHz
	T2CON = 0b00000110;
	CCPR1L = 64;
	CCP1CON = 0b00001100;

	scif_init();
	ei();

	printf("Thermometer and Hygrometer\n");
	printf("Temperature,Humidity\n");

	while(1){
		ADCON0 &= 0b11110011; // Select AN0
		__delay_us(20);
		GODONE = 1;
		while(GODONE);
		vt = (ADRESH << 8) | ADRESL;
		
		ADCON0 |= 0b00001100; // Select AN3
		__delay_us(20);
		GODONE = 1;
		while(GODONE);
		vh = (ADRESH << 8) | ADRESL;

		printf(
			"%d,%d\n",
			(int)(vt - 186) * 10 / 31,
			bresenham(vh + 133, 62, 770)
		);
		__delay_s(1);	
	}
}
