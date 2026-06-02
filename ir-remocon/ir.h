extern unsigned char ir_sig[];
extern unsigned char ir_ldrh;
extern unsigned char ir_ldrl;
extern unsigned char ir_turn;
extern unsigned char ir_sigh;
extern unsigned char ir_sigl;
extern unsigned char ir_scnt;

extern unsigned char tl, ts;
extern unsigned char hl, hs;
extern unsigned char ll, ls;

void ir_init(void);
void ir_save(void);
void ir_load(void);
void ir_analize(void);
void ir_shot(unsigned char);
void ir_control(void);
