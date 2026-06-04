/*	LCD-SDCARD UNIT
	I2C Master API
	Microchip Technology PIC24FJ64GA002
	Microchip Technology C30
*/

void i2c1_init(void);
void i2c1_start(void);
void i2c1_repstart(void);
void i2c1_stop(void);
unsigned char i2c1_write(unsigned char);
unsigned char i2c1_read(unsigned char);
void i2c2_init(void);
void i2c2_start(void);
void i2c2_repstart(void);
void i2c2_stop(void);
unsigned char i2c2_write(unsigned char);
unsigned char i2c2_read(unsigned char);
