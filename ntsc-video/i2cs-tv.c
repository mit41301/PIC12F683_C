/*	Filename: i2cs-tv.c
	Description: I2C slave API
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, NTSC-VIDEO UNIT
*/

#include <p24FJ64GA002.h>
#include "tv.h"

// System definition
#define I2CS_ADR 82

// Command cue definition
#define CUE_SIZE 16
enum {
	COM_POINT, // 0
	COM_LINE, // 1
	COM_LOCATE, // 2
	COM_PUTCH, // 3
	COM_PUTHC // 4
};

// Command cue and parameters
unsigned char com_cue[CUE_SIZE][5];

unsigned char i2cs_stage;
unsigned char i2cs_dummy;
unsigned char com_wp;
unsigned char com_rp;
unsigned char com_cc;

void i2cs_tv_init(void){
	// Parameter initialize
	com_wp = 0;
	com_rp = 0;
	com_cc = 0;

	// Pin function initialize
	AD1PCFGbits.PCFG4 = 1;
	AD1PCFGbits.PCFG5 = 1;

	// Resistor initialize
	I2C2ADD = I2CS_ADR;
	I2C2CON = 0b1000000000000000;
	IPC12bits.SI2C2P = 3; //priority !!!
	IFS3bits.SI2C2IF = 0;
	IEC3bits.SI2C2IE = 1;
}

void __attribute__((interrupt, no_auto_psv, shadow))
_SI2C2Interrupt(void){
	IFS3bits.SI2C2IF = 0;
	if(com_cc >= CUE_SIZE) return;

	if(I2C2STATbits.D_A == 0){
		i2cs_dummy = I2C2RCV;

	// Address, Master -> Slave
		if(I2C2STATbits.R_W == 0){
			i2cs_stage = 0;
			com_cc++;
			com_wp++;
			com_wp &= CUE_SIZE - 1;
		}

	// Address, Master <- Slave
		else {
			I2C2TRN = com_cc;
			I2C2CONbits.SCLREL = 1;
		}
	}
	else {

	// Data, Buffer full
		if(I2C2STATbits.RBF){
			if(i2cs_stage < 5){
				com_cue[com_wp][i2cs_stage] 
					= I2C2RCV;
				i2cs_stage++;
			} else {
				i2cs_dummy = I2C2RCV;
			}
		} else {

	// Data, Buffer empty, ACK
			if(I2C2STATbits.ACKSTAT == 0){
				I2C2TRN = com_cc;
				I2C2CONbits.SCLREL = 1;
			}

	// Data, Buffer empty, NACK
			else {
				;
			}
		}
	}
}

void i2cs_com_exe(void){
	if(com_cc == 0) return;
	while(I2C2STATbits.P == 0);

	// Command cue update
	IEC1bits.SI2C1IE = 0;
	com_cc--;
	com_rp++;
	com_rp &= CUE_SIZE - 1;
	IEC1bits.SI2C1IE = 1;

	// Command execute
	switch(com_cue[com_rp][0]){
	case COM_POINT:
		tv_point(
		(unsigned int)com_cue[com_rp][1],
		(unsigned int)com_cue[com_rp][2]
		);
		break;
	case COM_LINE:
		tv_line(
		(unsigned int)com_cue[com_rp][1],
		(unsigned int)com_cue[com_rp][2],
		(unsigned int)com_cue[com_rp][3],
		(unsigned int)com_cue[com_rp][4]
		);
		break;
	case COM_LOCATE:
		tv_locate(
		(char)com_cue[com_rp][1],
		(char)com_cue[com_rp][2]
		);
		break;
	case COM_PUTCH:
		tv_putch(
		(char)com_cue[com_rp][1]
		);
		break;
	case COM_PUTHC:
		tv_puthc(
		(char)com_cue[com_rp][1]
		);
		break;
	default:
		break;
	}
}
