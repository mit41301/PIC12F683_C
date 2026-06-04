/*	Filename: i2cs-led7.c
	Description: 7segment LED with I2C slave API
	Language: HI-TECH C PRO(Lite mode)
	Target: PIC16F88, 7SEG-LED UNIT
*/

#include <pic.h>
#include "font7s.h"

#define LED1 RB3
#define LED2 RB5
#define LED3 RB6
#define LED4 RB0
#define LED_DP RB2

unsigned char led7_dig;
unsigned char led7_ptn[4];
const unsigned char led7_sel[] = {
	0b00000001,
	0b01000000,
	0b00100000,
	0b00001000
};

#define I2CS_ADR 83

unsigned char i2cs_stage;
unsigned char i2cs_dummy;
unsigned char i2cs_d[3];

void led7_init(void){
	CMCON = 0b00000111;
	ANSEL &= 0b01000000;
	OPTION = 0b10000100;
	PORTA = 0;
	TRISA = 0b00000000; // out
	PORTB = 0;
	TRISB &= 0b10010010;

	led7_dig = 0;

	led7_ptn[3] = FONT7S_r;
	led7_ptn[2] = FONT7S_d;
	led7_ptn[1] = FONT7S_y;
	led7_ptn[0] = 0b00100100; // !

	TMR0IE = 1;
}

void led7_putui(unsigned int ui){
	unsigned char i;

	for(i = 0; i < 4; i++){
		led7_ptn[i] = font7s[ui % 10];
		ui /= 10;
	}
}

void led7_dot(unsigned char led7_dig){
	led7_ptn[0] &= 0b11011111;
	led7_ptn[1] &= 0b11011111;
	led7_ptn[2] &= 0b11011111;
	led7_ptn[3] &= 0b11011111;
	if(led7_dig < 4)
		led7_ptn[led7_dig] |= 0b00100000;
}

void i2cs_init(void){
	i2cs_stage = 0;

	TRISB1 = 1;
	TRISB4 = 1;
	SSPSTAT = 0b00000000;
	SSPADD  = I2CS_ADR << 1;
	SSPCON  = 0b00110110;

	SSPIF = 0;
	SSPIE = 1;
}

void i2cs_sync(void){
	led7_putui(
		(i2cs_d[0] << 8) | i2cs_d[1]
	);
	led7_dot(i2cs_d[2]);
}

void interrupt entry(void){
	if(TMR0IF){
		TMR0IF = 0;

		PORTB &= ~led7_sel[led7_dig];
		LED_DP = 0;

		led7_dig++;
		led7_dig &= 0b00000011;
		PORTA = led7_ptn[led7_dig];

		if(led7_ptn[led7_dig] & 0b00100000)
			LED_DP = 1;

		PORTB |= led7_sel[led7_dig];
	}
	else
	if(SSPIF){
		SSPIF = 0;

		if(DA == 0){ // case ADDRESS
		i2cs_dummy = SSPBUF;
			if(RW == 0){ // ADDRESS + W
				i2cs_stage = 0;
			} else { // ADDRESS + R
				SSPBUF = 0;
				CKP = 1;
			}
		} else { // case DATA
			if(BF){ // Master -> Slave
				if(i2cs_stage < 3){
					i2cs_d[i2cs_stage++] = SSPBUF;
				} else {
					i2cs_dummy = SSPBUF;
				}
			} else {
				if(CKP){ // ACK = 1
					;
				} else { // ACK = 0
					SSPBUF = 0;
					CKP = 1;
				}
			}
		}
	}
}
