/*	Filename: ir.c
	Description: Ir Signal API
	Language: HI-TECH C PRO(Lite mode)
	Target: PIC12F683, IR-REMOCON UNIT
*/

#include <pic.h>

#define _XTAL_FREQ 8000000
#define IR_IN  GPIO0
#define IR_OUT GPIO4
#define IR_MAX 16

unsigned char ir_sig[IR_MAX];
unsigned char ir_scnt;
unsigned char ir_ldrh;
unsigned char ir_ldrl;
unsigned char ir_turn;
unsigned char ir_sigh;
unsigned char ir_sigl;

unsigned char tl, ts;
unsigned char hl, hs;
unsigned char ll, ls;

void ir_init(void){
	CMCON0 = 0b00000111;
	ANS0  = 0;
	ANS3  = 0;
	WPU0 = 0;
	TRISIO0 = 1; 
	TRISIO4 = 0;

	IR_OUT = 0;
}

void ir_save(void){
	unsigned char i;

	eeprom_write(0, ir_scnt);
	eeprom_write(1, ir_ldrh);
	eeprom_write(2, ir_ldrl);
	eeprom_write(3, ir_turn);
	eeprom_write(4, ir_sigh);
	eeprom_write(5, ir_sigl);
	for(i = 0; i < ir_scnt; i++)
		eeprom_write(i + 6, ir_sig[i]);
}

void ir_load(void){
	unsigned char i;

	ir_scnt = eeprom_read(0);
	ir_ldrh = eeprom_read(1);
	ir_ldrl = eeprom_read(2);
	ir_turn = eeprom_read(3);
	ir_sigh = eeprom_read(4);
	ir_sigl = eeprom_read(5);
	for(i = 0; i < ir_scnt; i++)
		ir_sig[i] = eeprom_read(i + 6);
}

void ir_shot(unsigned char t){
	TMR0 = 0;
	while(TMR0 < t){
		IR_OUT = 1;
		if(ir_ldrh < 100) _delay(17);
		else _delay(9);
		IR_OUT = 0;
		if(ir_ldrh < 100) _delay(7);
		else _delay(17);
	}
	TMR0 = 0;
}

void ir_control(void){
	unsigned char i;
	unsigned char b;

	// Shot reader
	OPTION = 0b00000110;
	ir_shot(ir_ldrh);
	while(TMR0 < ir_ldrl);

	// Shot data
	OPTION = 0b00000011;
	for(i = 0; i < ir_scnt; i++){
		for(b = 0; b < 8; b++){
			ir_shot(ir_turn);
			if((ir_sig[i] & (0x80 >> b)) > 0){
				while(TMR0 < ir_sigh);
			} else {
				while(TMR0 < ir_sigl);
			}
		}
	}
	ir_shot(ir_turn); // stop bit
}

void ir_analize(void){
	unsigned char i, b;
	unsigned char sp;

	tl = 0; ts = 255;
	hl = 0; hs = 255;
	ll = 0; ls = 255;

	OPTION = 0b00000110;
	TMR0 = 0;
	while(TMR0 < 40) if(IR_IN) TMR0 = 0;
	while(IR_IN == 0);
	ir_ldrh = TMR0;

	TMR0 = 0;
	while(IR_IN);
	ir_ldrl = TMR0;

	OPTION = 0b00000011;
	for(i = 0; i < IR_MAX; i++){
		for(b = 0; b < 8; b++){
			TMR0 = 0;
			while(IR_IN == 0);
			ir_turn = TMR0;
			TMR0 = 0;
			if(ir_turn > tl) tl = ir_turn;
			else if(ir_turn < ts) ts = ir_turn;

			while(IR_IN && (TMR0 < 240));
			sp = TMR0;
			if(sp >= 240) break;
			ir_sig[i] <<= 1; 		
			if(sp > 100){
				ir_sig[i] |= 1;
				if(sp > hl) hl = sp;
				else if(sp < hs) hs = sp;
			} else {
				if(sp > ll) ll = sp;
				else if(sp < ls) ls = sp;
			}
		}
		if(sp >= 240) break;
	}
	ir_scnt = i;
	ir_turn = ((tl >> 1) + (ts >> 1));
	ir_sigh = ((hl >> 1) + (hs >> 1)) ;
	ir_sigl = ((ll >> 1) + (ls >> 1));
}
