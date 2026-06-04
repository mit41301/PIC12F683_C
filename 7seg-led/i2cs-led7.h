#define FONT7S_MINUS 0b10000000
#define FONT7S_BLANK 0b00000000
#define FONT7S_A 0b11001111
#define FONT7S_a 0b11011110
#define FONT7S_b 0b11011001
#define FONT7S_C 0b01010011
#define FONT7S_c 0b11010000
#define FONT7S_d 0b11011100
#define FONT7S_E 0b11010011
#define FONT7S_F 0b11000011
#define FONT7S_n 0b11001000
#define FONT7S_o 0b11011000
#define FONT7S_r 0b11000000
#define FONT7S_y 0b10011101

extern unsigned char led7_ptn[];
void led7_putui(unsigned int);
void led7_dot(unsigned char);
void led7_init(void);

void i2cs_init(void);
void i2cs_sync(void);
