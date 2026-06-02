/*	Filename: scif_tx.c
	Description: UART TX API
	Language: HI-TECH C PRO(Lite mode)
	Target: PIC12F683, IR-REMOCON UNIT
*/

#include <pic.h>

#define SCIF_TX_OUT  GPIO1
#define SCIF_TX_TM 206 // 1200bps@8MHz
#define SCIF_TX_BN 11

unsigned char scif_tx_bit;
unsigned char scif_tx_buf;
bit scif_tx_next;

void scif_tx_init(void){
	T1CON =  0b00110001;
	CMCON0 = 0b00000111;
	ANS1 = 0;
	SCIF_TX_OUT = 1;
	TRISIO1 = 0;
}

void putch(unsigned char c){
	scif_tx_next = 0;
	scif_tx_buf = c;
	scif_tx_bit = SCIF_TX_BN;
	do{
		while(TMR1L < SCIF_TX_TM);
		SCIF_TX_OUT = scif_tx_next;
		TMR1L = 0;
		scif_tx_next = scif_tx_buf & 1;
		scif_tx_buf = (scif_tx_buf >> 1) | 0x80;
	} while(--scif_tx_bit);
}

void puthc(unsigned char d){
	const unsigned char hc[] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'A', 'B',
		'C', 'D', 'E', 'F'
	};

	putch(hc[d >> 4]);
	putch(hc[d & 0x0f]);
}

void putui(unsigned int ui, unsigned char d){

	unsigned char i;
	unsigned char buf[5];

	for(i = 0; i < 5; i++) buf[i] = ' ';
	i = 4;
	do {
		buf[i] = (ui % 10) + '0';
		ui = ui / 10;
		i--;
	} while(ui > 0);

	for(i = (5 - d); i < 5; i++) putch(buf[i]);
}

void puts(const unsigned char *s){
	while(*s) putch(*s++);
}

void deb_sig(const unsigned char *s, unsigned int d){
	puts(s);
	putch('=');
	putui(d, 3);
	putch('\n');
}

void deb_dat(unsigned int d, unsigned char h){
	puts("Data No.");
	putui(d, 2);
	putch('=');
	puthc(h);
	putch('\n');
}
