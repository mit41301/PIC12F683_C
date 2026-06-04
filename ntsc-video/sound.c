/*	Filename: sound.c
	Description: Sound API
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, NTSC-VIDEO UNIT
*/

#include <p24FJ64GA002.h>

#define SOUND_PIN PORTBbits.RB15

void sound_init(void){
	AD1PCFGbits.PCFG9 = 1;
	PMCONbits.PMPEN = 0;
	TRISBbits.TRISB15 = 0;
}

void sound_out(unsigned int t){
	unsigned int i;

	while(t--){
		SOUND_PIN = 1;
		i = 400;
		while(i--) Nop();
		SOUND_PIN = 0;
		i = 400;
		while(i--) Nop();
	}
}
