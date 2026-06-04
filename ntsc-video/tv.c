/*	Filename: tv.c
	Description: Video Control API
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, NTSC-VIDEO UNIT
*/

#include <p24FJ64GA002.h>
#include "tv_font.h"

#define TV_D_RES 241
#define TV_V_SYN 19 // 20@typ
#define TV_V_SPC 24
#define TV_H_SPC 111

#define TV_V_RES 192
#define TV_H_RES 256 
#define TCY_H_W 1016 // 63.5usec
#define TCY_H_SPW 75 // 4.7usec

#define TV_S_OUT	LATBbits.LATB11
#define TV_V_OUT	LATBbits.LATB10

unsigned char tv_disp;
unsigned char tv_vsyn;

unsigned int __attribute__((far))
tv_vram[TV_V_RES * (TV_H_RES / 16)];
unsigned int *rp_vram;
unsigned int rp_follow; 

unsigned char tv_cx, tv_cy;

void tv_init(void){
	unsigned int i;

	TRISBbits.TRISB11 = 0;
	TRISBbits.TRISB10 = 0;
	RPOR5bits.RP10R = 7;

	T2CON = 0x0000;
	PR2 = TCY_H_W - 1;
	IPC1bits.T2IP = 4;
	IFS0bits.T2IF = 0;
	IEC0bits.T2IE = 1;

	IPC0bits.OC1IP = 4;
	IFS0bits.OC1IF = 0;
	IEC0bits.OC1IE = 1;

	IPC1bits.OC2IP = 4;
	IFS0bits.OC2IF = 0;
	IEC0bits.OC2IE = 1;

	SPI1CON1 = 0x0437;
	SPI1CON2 = 0;
	SPI1STAT = 0x8000;
	IPC2bits.SPI1IP = 4;
	IFS0bits.SPI1IF = 0;
	IEC0bits.SPI1IE = 1;

	tv_disp = 0;
	tv_vsyn = TV_V_SYN;
	rp_vram = tv_vram;
	for(i = 0; i < 3072; i++) *rp_vram++ = 0;
	rp_vram = tv_vram;

	tv_cx = tv_cy = 0;

	T2CONbits.TON = 1;
}

void __attribute__((interrupt,no_auto_psv, shadow))
_T2Interrupt(void){
	TV_S_OUT = 0;
	if(tv_disp > TV_D_RES){
		OC1R = TCY_H_W - TCY_H_SPW - 5;
		OC1CON = 0x0001;
		tv_vsyn--;
		if(tv_vsyn == 0){
			tv_disp = 0;
			tv_vsyn = TV_V_SYN;
			rp_vram = tv_vram;				}
	} else {
		OC1R = TCY_H_SPW - 5;
		OC1CON = 0x0001;
		if(
			(tv_disp > TV_V_SPC) && 
			(tv_disp < TV_V_SPC + TV_V_RES)
		){
			OC2R = TCY_H_SPW + TV_H_SPC;
			OC2CON = 0x0001;
		}
		tv_disp++;
	}
	IFS0bits.T2IF = 0;
}

void __attribute__((interrupt, no_auto_psv, shadow)) 
_OC1Interrupt(void)
{
	TV_S_OUT = 1;
	IFS0bits.OC1IF = 0;
}

void __attribute__((interrupt, no_auto_psv, shadow)) 
_OC2Interrupt(void)
{
	SPI1BUF = *rp_vram++;
	IFS0bits.OC2IF = 0;
	rp_follow = 0;
	while(SPI1STATbits.SPITBF);
	SPI1BUF = *rp_vram++;
}

void __attribute__((interrupt, no_auto_psv, shadow)) 
_SPI1Interrupt(void)
{
	int dumy;

	if(rp_follow < 14){
		SPI1BUF = *rp_vram++;
		rp_follow++;
	}
	dumy = SPI1BUF;
	IFS0bits.SPI1IF = 0;
}

void tv_point(unsigned int x, unsigned int y)
{
	if((x < TV_H_RES) && (y < TV_V_RES))
		tv_vram[(y << 4) + (x >> 4)] |=
			(0x8000 >> (x & 0x000f));
}

void tv_line(
	unsigned int x1,
	unsigned int y1,
	unsigned int x2,
	unsigned int y2
){
	unsigned int dx, dy;
	int sx, sy;
	unsigned char chg;
	int e;
	unsigned int i, tmp;

	if(x1 > x2){
		sx = -1; dx = x1 - x2;
	} else {
		sx =  1; dx = x2 - x1;
	}
	if(y1 > y2){
		sy = -1; dy = y1 - y2;
	} else {
		sy =  1; dy = y2 - y1;
	}

	if(dy > dx){
		tmp = dx; dx = dy; dy = tmp;
		chg = 1;
	} else 
		chg = 0;

	e = (dy << 1) - dx;
	for(i = 0; i <= dx; i++){
		tv_point(x1, y1);
		if(e >= 0){
			if(chg == 1)
				x1 += sx;
			else
				y1 += sy;
			e -= (dx << 1);
		}
		if(chg == 1)
			y1 += sy;
		else
			x1 += sx;
		e += (dy << 1);
	}
}

void tv_putch(char c){
	unsigned int i;
	unsigned int *pv;
	const unsigned char *pf;

	c -= FONT_OFS;
	if ((c < 0) || (c >= FONT_CNT)) c = 0;

	if(tv_cx >= TV_H_RES / FONT_H){
		tv_cx = 0;
		tv_cy++;   
	}
	if(tv_cy >= TV_V_RES / FONT_V) tv_cy = 0;

	pv = &tv_vram[
	tv_cy * FONT_V * TV_H_RES / 16 + tv_cx / 2
	];
	pf = &tv_font[c * FONT_V];

	for(i = 0; i < FONT_V; i++){
		if(tv_cx & 1){
			*pv &= 0xff00;
			*pv |= (int)(*pf++);
		} else {
			*pv &= 0x00ff;
			*pv |= (int)(*pf++) << 8;
		}
		pv += TV_H_RES / 16; 
	}
	tv_cx++;
}

void tv_puts(char *s){
	while(*s) tv_putch(*s++);
}

void tv_puthc(char d){
const unsigned char hc[] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'A', 'B',
	'C', 'D', 'E', 'F'
};
	tv_putch(hc[d >> 4]);
	tv_putch(hc[d & 0x0f]);
}

void tv_locate(unsigned char x, unsigned char y){
	if(
		(x < TV_H_RES / FONT_H) &&
		(TV_H_RES / FONT_V)
	){
		tv_cx = x;
		tv_cy = y;
	}
}
