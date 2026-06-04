/*
	NTSC-VIDEO UNIT
	DELAY API
	Microchip Technology PIC24FJ64GA002
	Microchip Technology C30
*/

#include <p24FJ64GA002.h>

void delay_us(unsigned int t){
	while(t--){
		Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop();
		Nop();
	}
}

void delay_ms(unsigned int t){
	while(t--) delay_us(1000);
}

void delay_s(unsigned int t){
	while(t--) delay_ms(1000);
}
