/*	Filename: i2cm.c
	Description: I2C master API
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, LCD-SDCARD UNIT
*/

#include <p24FJ64GA002.h>

void i2c2_init(void){
	AD1PCFGbits.PCFG4 = 1;
	AD1PCFGbits.PCFG5 = 1;

	I2C2BRG = 511; // 33kHz
	I2C2CON = 0b1000000000000000; // 7bit address
}

void i2c2_idleck(void){
	while(
	(I2C2CON & 0b0000000000011111) |
	I2C2STATbits.TRSTAT
	);
}

void i2c2_start(void){
	i2c2_idleck();
	I2C2CONbits.SEN = 1;
}

void i2c2_repstart(void){
	i2c2_idleck();
	I2C2CONbits.RSEN = 1;
}

void i2c2_stop(void){
	i2c2_idleck();
	I2C2CONbits.PEN = 1;
}

unsigned char
i2c2_write(unsigned char d){
	i2c2_idleck();
	I2C2TRN = d;
	i2c2_idleck();
	return(I2C2STATbits.ACKSTAT);
}

unsigned char
i2c2_read(unsigned char a){
	unsigned char d;

	i2c2_idleck();
	I2C2CONbits.RCEN = 1;
	i2c2_idleck();
	d = I2C2RCV;
	I2C2CONbits.ACKDT = a;
	I2C2CONbits.ACKEN = 1;
	return(d);
}
