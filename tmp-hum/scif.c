/*	Filename: scif.c
	Description: UART TX API
	Language: HI-TECH C PRO(Lite mode)
	Target: PIC12F683, TMP-HUM UNIT
*/

#include <pic.h>

#define SCIF_TM 59 // 1200bps
#define SCIF_TX_BN 11
#define SCIF_TX GPIO5
#define SCIF_RX GPIO3

unsigned char rx_state;
enum scif_rx_state {
	RX_MARK,
	RX_HALF,
	RX_DATA,
	RX_STOP = RX_DATA + 8
};

unsigned char skip_ovs; 
unsigned char rx_shift;
unsigned char tx_bit;
unsigned char tx_buf;
bit tx_next;

unsigned char scif_rb[16];
unsigned char scif_wp;
unsigned char scif_rp;
unsigned char scif_dc;

void scif_init(void){
	rx_state = RX_MARK;
	skip_ovs = 1;

	scif_wp = 0;
	scif_rp = 0;
	scif_dc = 0;

	CMCON0 = 0b00000111;
	TRISIO5 = 0;
	TRISIO3 = 1;
	OPTION = 0b10000000;
	TMR0 = SCIF_TM - 2;
	T0IF = 0;
	T0IE = 1;
}

void interrupt entry(void){
	if(T0IF){
		TMR0 = SCIF_TM;
		T0IF = 0;

	// Receive Process 
	if(--skip_ovs == 0){
		skip_ovs++; 
		switch(rx_state) {

		case RX_MARK:
		if(!SCIF_RX){
			skip_ovs = 2;
			rx_state++;
		}
		break;

		case RX_HALF:
		if(!SCIF_RX) {
			skip_ovs = 4;
			rx_state++;
		} else {
			rx_state = RX_MARK;
		}
		break;

		// case RX_DATA
		default:
		rx_shift = 
			(rx_shift >> 1) |
			(SCIF_RX << 7);
		skip_ovs = 4;
		rx_state++;
		break;

		case RX_STOP:
		rx_state = RX_MARK;

		if(scif_dc >= 16)
			break;

		scif_rb[scif_wp] = rx_shift;
		scif_wp++;
		scif_wp &= 15;
		scif_dc++;
		break;
		}
	}

	// Transmit Process 
	if(tx_bit) {
		if((tx_bit & 3) == 0) {
			SCIF_TX = tx_next;
			tx_next = tx_buf & 1;
			tx_buf = (tx_buf >> 1) | 0x80;
		}
		tx_bit--;
		}
	}
}

void putch(unsigned char c){
	while(tx_bit);
	tx_next = 0;
	tx_buf = c;
	tx_bit = SCIF_TX_BN * 4;
}

unsigned char getch(void){
	unsigned char c;

	while(scif_dc == 0);
	while(rx_state != RX_MARK);

	T0IE = 0;
	c = scif_rb[scif_rp];
	scif_rp++;
	scif_rp &= 15;
	scif_dc--;
	T0IE = 1;
	return(c);
}

bit kbhit(void){
	return(scif_dc);
}

unsigned char getche(void){
	unsigned char c;

	c = getch();
	if(c == '\r')
		c = getch();
	putch(c);
	return(c);
}
