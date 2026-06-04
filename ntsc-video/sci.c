/*	Filename: sci.c
	Description: UART API
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, NTSC-VIDEO UNIT
*/

#include <p24FJ64GA002.h>
#include <ctype.h>

#define XON 17
#define XOFF 19
#define BUF_SIZE 64

typedef struct {
	unsigned char xf;
	char buf[BUF_SIZE];
	unsigned char wp;
	unsigned char rp;
	unsigned char dc;
} RX_BUF;

RX_BUF u1rx;
RX_BUF u2rx;

void uart1_init(void)
{
	TRISBbits.TRISB8 = 0; // TX
	TRISBbits.TRISB9 = 1; // RX

	RPOR4bits.RP8R = 3; // 3 = U1TX
	RPINR18bits.U1RXR = 9; // 9 = RP9

	U1BRG = 103; // 9600bps
	U1MODE = 0b1000100000000000;
	U1STA =  0b0000010000000000;

	IPC2bits.U1RXIP = 4;
	IFS0bits.U1RXIF = 0;
	IEC0bits.U1RXIE = 1;
}

void putch1(char c){
	while(u1rx.xf == XOFF);
	while(U1STAbits.UTXBF);
	U1TXREG = c;
}

void puts1(char *s){
	while(*s) putch1(*s++);
	putch1('\n');
}

void __attribute__((interrupt, no_auto_psv, shadow)) _U1RXInterrupt(void){
	char c;

	IFS0bits.U1RXIF = 0;

	c = U1RXREG;
	if(c == XON || c == XOFF){
		u1rx.xf = c;
		return;
	}

	if(u1rx.dc == BUF_SIZE) return;

	u1rx.buf[u1rx.wp] = c;
	u1rx.wp++;
	u1rx.wp &= (BUF_SIZE - 1);
	u1rx.dc++;
	if(u1rx.dc == (BUF_SIZE * 3 / 4)){
		while(U1STAbits.UTXBF);
		U1TXREG = XOFF;
	}
}

unsigned char kbhit1(void){
	return(u1rx.dc);
}

char getch1(void){
	char c;

	while(!kbhit1());
	IEC0bits.U1RXIE = 0; //disable
	c = u1rx.buf[u1rx.rp];
	u1rx.rp++;
	u1rx.rp &= (BUF_SIZE - 1);
	u1rx.dc --;
	if(u1rx.dc == (BUF_SIZE * 3 / 4)){
		while(U1STAbits.UTXBF);
		U1TXREG = XON;
	}
	IEC0bits.U1RXIE = 1; //enable
	return(c);
}

void gets1(char *buf, unsigned char max)
{
	char c;
	unsigned char len;

	len = 0;
	while((c = getch1()) != '\n'){
		if( c == 9) c = ' ';
		if((c == 8) && (len > 0)){
			len--;
			putch1(8); putch1(' '); putch1(8);
		} else 
		if(isprint(c) && (len < max)){
			buf[len++] = c;
			putch1(c);
		}
	}
	buf[len] = 0;
	putch1('\n');
}

/*	PICkit2 UART TOOL interface
	TX-RP0/RB0
	RX-RP1/RB1
*/

void uart2_init(void)
{
	TRISBbits.TRISB0 = 0; // TX
	TRISBbits.TRISB1 = 1; // RX

	RPOR0bits.RP0R = 5; // 5 = U2TX
	RPINR19bits.U2RXR = 1; // 1 = RP1

	U2BRG = 103; // 9600bps
	U2MODE = 0b1000100000000000;
	U2STA =  0b0000010000000000;

	IPC7bits.U2RXIP = 4;
	IFS1bits.U2RXIF = 0;
	IEC1bits.U2RXIE = 1;
}

void putch2(char c){
	while(U2STAbits.UTXBF);
	U2TXREG = c;
}

void puts2(char *s){
	while(*s) putch2(*s++);
	//putch2('\n');
}

void __attribute__((interrupt, no_auto_psv, shadow)) _U2RXInterrupt(void){
	char c;

	IFS1bits.U2RXIF = 0;
	c = U2RXREG;

	if(u2rx.dc == BUF_SIZE) return;

	u2rx.buf[u2rx.wp] = c;
	u2rx.wp++;
	u2rx.wp &= (BUF_SIZE - 1);
	u2rx.dc++;
}

unsigned char kbhit2(void){
	return(u2rx.dc);
}

char getch2(void){
	char c;

	while(!kbhit2());
	IEC1bits.U2RXIE = 0; //disable
	c = u2rx.buf[u2rx.rp];
	u2rx.rp++;
	u2rx.rp &= (BUF_SIZE - 1);
	u2rx.dc --;
	IEC1bits.U2RXIE = 1; //enable
	return(c);
}

void gets2(char *buf, unsigned char max)
{
	char c;
	unsigned char len;

	len = 0;
	while((c = getch2()) != '\n'){
		if( c == 9) c = ' ';
		if((c == 8) && (len > 0)){
			len--;
			putch2(8); putch2(' '); putch2(8);
		} else 
		if(isprint(c) && (len < max)){
			buf[len++] = c;
			putch2(c);
		}
	}
	buf[len] = 0;
	putch2('\n');
}
