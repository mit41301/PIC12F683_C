/*	Filename: ir-remocon.c
	Description: Application Example
	Language: HI-TECH C PRO(Lite mode)
	Target: PIC12F683, IR-REMOCON UNIT
*/

#include <pic.h>
#include "scif_tx.h"
#include "ir.h"

__CONFIG(
	  MCLRDIS
	& PWRTEN
	& BORDIS
	& UNPROTECT
	& WDTDIS
	& INTIO
	& FCMDIS
	& IESODIS
);

__IDLOC(F683);

#define _XTAL_FREQ 8000000
#define SWTCH GPIO2
#define LED GPIO5

void interrupt entry(void){
	if(INTF)
		INTF = 0;
}

void main(void){
	unsigned char i;

	OSCCON = 0b01110000; // 8MHz

	TRISIO5 = 0;
	LED = 1;

	GPPU = 0;
	WPU2 = 1;
	ANS2 = 0;
	TRISIO2 = 1;
	__delay_ms(10);
	INTF = 0;
	INTE = 1;
	ei();

	ir_init();
	scif_tx_init();

	if(SWTCH == 0){
		puts("Ir catch ready.\n");
		ir_analize();

		LED = 0;
		ir_save();

		// Output for debug
		puts("Format = ");
		if(ir_ldrh < 100)
			puts("Kaden kyokai\n");
		else
			puts("NEC\n");
		deb_sig("Signal count", ir_scnt);
		deb_sig("Leader H/8", ir_ldrh);
		deb_sig("Leader L/8", ir_ldrl);
		deb_sig("Tern max", tl);
		deb_sig("Tern min", ts);
		deb_sig("Tern avg", ir_turn);
		deb_sig("Signal H max", hl);
		deb_sig("Signal H min", hs);
		deb_sig("Signal H avg", ir_sigh);
		deb_sig("Signal L max", ll);
		deb_sig("Signal L min", ls);
		deb_sig("Signal L avg", ir_sigl);
		for(i = 0; i < ir_scnt; i++){
			deb_dat(i, ir_sig[i]);
		}
		LED = 1; 
	}

	LED = 0;
	ir_load();
	LED = 1; 
 
	while(1){
		SLEEP();
		ir_control();
	}
}
