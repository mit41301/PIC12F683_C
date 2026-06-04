/*	Filename: lcd.c
	Description: LCD API
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, NTSC-VIDEO UNIT
*/

#include <p24FJ64GA002.h>

#define LCD_E  LATBbits.LATB14
#define LCD_RW LATBbits.LATB13
#define LCD_RS LATBbits.LATB15
#define LCD_BF PORTBbits.RB3

#define DELAY_TW() Nop();Nop();Nop();Nop()

void delay_us(unsigned int t){
	while(t--){
		Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop();
	}
}

void delay_ms(unsigned int t){
	while(t--) delay_us(1000);
}

void lcd_w_com4(unsigned char d){
	LATBbits.LATB0 = (d     ) & 1;
	LATBbits.LATB1 = (d >> 1) & 1;
	LATBbits.LATB2 = (d >> 2) & 1;
	LATBbits.LATB3 = (d >> 3) & 1;
	LCD_RW = 0;
	LCD_RS = 0;

	LCD_E = 1; DELAY_TW(); LCD_E = 0;
}

void lcd_w_chr4(unsigned char d){
	LATBbits.LATB0 = (d     ) & 1;
	LATBbits.LATB1 = (d >> 1) & 1;
	LATBbits.LATB2 = (d >> 2) & 1;
	LATBbits.LATB3 = (d >> 3) & 1;
	LCD_RW = 0;
	LCD_RS = 1;

	LCD_E = 1; DELAY_TW(); LCD_E = 0;
}

void lcd_bfck(void){
	unsigned char bf;

	TRISB |= 0x000f;
	LCD_RW = 1;
	LCD_RS = 0;

	do{
		LCD_E = 1; DELAY_TW();
		bf = LCD_BF;
		LCD_E = 0; DELAY_TW();

		LCD_E = 1; DELAY_TW();
		LCD_E = 0; DELAY_TW();
	} while(bf);

	TRISB &= 0xfff0;
}

void lcd_putcom(unsigned char d){
	lcd_bfck();
	lcd_w_com4(d >> 4);
	lcd_w_com4(d);
}

void lcd_locate(unsigned char x, unsigned char y){
	if(y > 0) x += 0x40;
	lcd_putcom(x | 0x80);
}

void lcd_putch(char d){
	lcd_bfck();
	lcd_w_chr4(d >> 4);
	lcd_w_chr4(d);
}

void lcd_puts(char *s){
	while(*s) lcd_putch(*s++);
}

void lcd_lclr(unsigned char d){
	unsigned char i;

	lcd_locate(0, d);
	for(i = 0; i < 40; i++)
		lcd_putch(' ');
	lcd_locate(0, d);
}

void lcd_puthc(unsigned char d){
	const char hc[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F'
	};

	lcd_putch(hc[d >> 4]);
	lcd_putch(hc[d & 0b00001111]);
}

void lcd_putui(unsigned int ui, unsigned char d){
	unsigned char i;
	unsigned char buf[5];

	for(i = 0; i < 5; i++) buf[i] = ' ';
	i = 4;
	do {
		buf[i] = (ui % 10) + '0';
		ui /= 10;
		i--;
	} while(ui > 0);

	for(i = (5 - d); i < 5; i++) lcd_putch(buf[i]);
}

void lcd_msg(char*s0, char*s1){
	lcd_lclr(0);
	lcd_puts(s0);
	lcd_lclr(1);
	lcd_puts(s1);
}

void lcd_init(void){
	AD1PCFG |= 0b0000111000111100;
	TRISB   &= 0b0001111111110000;

	LCD_E = 0;

	delay_ms(15);
	lcd_w_com4(0b00000011); delay_ms(5);
	lcd_w_com4(0b00000011); delay_us(100);
	lcd_w_com4(0b00000011); delay_us(100);
	lcd_w_com4(0b00000010);

	lcd_putcom(0b00101000);
	lcd_putcom(0b00000001);
	lcd_putcom(0b00001100);
}
