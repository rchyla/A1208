/* A1208 configuration program */
/* Initial software structure by Radosław "strim" Kujawa(https://github.com/rkujawa)
/* modded/adapted by Rafał. G. Chyła vel Sanjyuubi */

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>
#include <exec/execbase.h>
#include <workbench/startup.h>

#include <proto/dos.h>
#include <proto/exec.h>

#include "hardware.h"
#include "addmem.h"
#include "cfgreg.h"


#define EXIT_WARNING		5
#define EXIT_SYNTAX_ERROR	10
#define EXIT_HARDWARE_ERROR	20 


/* -- function prototypes -- */

void reboot(void);
void status_display(void);


/* -- global variables -- */

static const STRPTR version = "\0$VER: Ver 1.0 (2019-01-13)\0";
static const STRPTR id = "\0$Id: 1bd186d77dd243a27c26267de8870027047f7101 $\0";

static LONG *argArray;	/* arguments passed on the command line */

BOOL debug = FALSE;

UBYTE r0, r1, r2, r3;

/* -- implementation -- */


void
status_print_reg_inv(UBYTE reg, UBYTE bit)
{
	if (reg & bit)
		printf("disabled\n");
	else
		printf("enabled\n");
}

void
status_print_reg(UBYTE reg, UBYTE bit)
{
	if (reg & bit)
		printf("enabled\n");
	else
		printf("disabled\n");
}

/* read and display configuration registers */
void
status_display(void) 
{

	printf(" ===============================================\n");
	printf("  A1208 by Lukzer rev:\t%d.%d\n", (r2 >> 4), (r3 >> 4));
	printf(" ================== Card status ================ \n");
	printf("\n");
	
	printf("  Expansion:\t\t");
	status_print_reg(r0, CFG_CARD_STAT);
	
	printf("  PCMCIA jumper:\t");
	status_print_reg(r0, CFG_JP_STAT);
	
	printf("  PCMCIA friendly(4MB):\t");
	status_print_reg(r0, CFG_CC_STAT);

	printf("  P-SLOW memory:\t");
	status_print_reg(r0, CFG_PSLOWMEM_STAT);
	
	
	printf("\n");
	printf(" ================== Control bits  ============== \n");
	printf("\n");

	printf("  Expansion:\t\t");
	status_print_reg(r1, CFG_CARD_EN);

	printf("  Jumper invert:\t");
	status_print_reg(r1, CFG_JP_INVERT);
	
	printf("  P-SLOW memory:\t");
	status_print_reg(r1, CFG_PSLOWMEM);
	
	printf("  Extra memory:\t\t");
	status_print_reg(r1, CFG_EXTRAMEM);
	
	printf("\n");
	
	printf(" =============================================== \n");
	printf(" Type \"A1208cfg help\" for more info.\n\n");



}

void
help(void) 
{

	printf("Possible arguments:\n");
	printf("help\t  - this help\n");
	printf("card\t  - enable/disable card\n");
	printf("pcmcia\t  - re-configures jumper internally to fit demand\n");
	printf("pslow\t  - enable 1.5MB of SLOW, pcmcia mode only\n");
	printf("extramem  - additional non-config memory, pcmcia mode only\n");
	printf("\n");
	printf("Example:\n");
	printf("A1208cfg card on pcmcia on pslow on\n");
	printf("A1208cfg extramem\n");
	printf("\n");
	printf("Type A1208cfg without arguments to view status.\n");
	printf("\n");

	
}

BOOL
arg_switch_isempty(UBYTE argNo)
{
	if ( ((LONG) argArray[argNo] != 0))
		return FALSE;

	return TRUE;
}

BOOL
arg_toggle_val(UBYTE argNo)
{
#define TOGGLE_EMPTY	-2
#define TOGGLE_FALSE	0x0
#define TOGGLE_TRUE	0xFFFFFFFF
	if ((LONG) argArray[argNo] == TOGGLE_TRUE)
		return TRUE;
	else if ((LONG) argArray[argNo] == TOGGLE_FALSE)
		return FALSE;
/*	else
		// I wonder if we'll observe one of these, duh. 
		printf("DEBUG: toggle neither TRUE nor FALSE, this should not happen!\n");
*/

	return FALSE;
}

BOOL
arg_key_isempty(UBYTE argNo)
{
	if ((LONG) argArray[argNo] == 0)
		return TRUE;
	else
		return FALSE;
}

BOOL
arg_toggle_isempty(UBYTE argNo)
{
	if ((LONG) argArray[argNo] != TOGGLE_EMPTY)
		return FALSE;	
	else
		return TRUE;
}


int
main(int argc, char *argv[])
{
	/*
	 * AmigaOS ReadArgs-style argument parsing is an inconsistent shit.
	 * Pile of shit. Period.
	 */
	UBYTE returnCode = 0;
	UBYTE returnCode_EXIT = 0;
	
	struct RDArgs *result;
	CONST_STRPTR argTemplate =
	    "CARD/T,PCMCIA/T,PSLOW/T,EXTRAMEM/S,HELP/S,DEBUG/S";
#define ARGNUM		6

#define CARD_ARG		0
#define PCMCIA_ARG		1
#define PSLOW_ARG		2
#define EXTRAMEM_ARG	3
#define HELP_ARG		4
#define DEBUG_ARG		5

	argArray = AllocVec(ARGNUM*sizeof(LONG), MEMF_ANY|MEMF_CLEAR);

	argArray[CARD_ARG] = TOGGLE_EMPTY;
	argArray[PCMCIA_ARG] = TOGGLE_EMPTY;
	argArray[PSLOW_ARG] = TOGGLE_EMPTY;
	

	result = ReadArgs(argTemplate, argArray, NULL);

	if (!arg_switch_isempty(DEBUG_ARG)) {
		debug = TRUE; 
	}

	if (!arg_switch_isempty(HELP_ARG)) {
		help();
		return 0;
	}





	if (HW_detect() == -1) {
		printf("Lukzer's A1208 board not detected! :(\n");
		return EXIT_HARDWARE_ERROR;
	}
	
	/* load registers*/
	r0 = cfgreg_read(CFG_STATUS); 	/* STATUS */
	r1 = cfgreg_read(CFG_CONTROL); 	/* CONTROL */
	r2 = cfgreg_read(CFG_REV); 		/* REVISION */
	r3 = cfgreg_read(CFG_SUBREV); 	/* SUBREVISION */

	cfgreg_unlock();
	

	if (!arg_toggle_isempty(CARD_ARG)) {
		if (arg_toggle_val(CARD_ARG)) 
			cfgreg_set(CFG_CONTROL, CFG_CARD_EN);
		else
			cfgreg_unset(CFG_CONTROL, CFG_CARD_EN);
	}



	if (!arg_toggle_isempty(PCMCIA_ARG)) {
		if (arg_toggle_val(PCMCIA_ARG)) { /*if PCMCIA ON request */
			/* if jumper is OFF (1) */
			if(r0 & CFG_JP_STAT) cfgreg_unset(CFG_CONTROL, CFG_JP_INVERT); /* turn invertion on*/
			else cfgreg_set(CFG_CONTROL, CFG_JP_INVERT); /* turn invertion off */
		}
		
		
		else{/*if PCMCIA OFF request*/
			/*if jumper is OFF (1)*/
			if(r0 & CFG_JP_STAT) cfgreg_set(CFG_CONTROL, CFG_JP_INVERT); /*turn invertion off*/
			else cfgreg_unset(CFG_CONTROL, CFG_JP_INVERT); /*turn invetion on*/
			
		
		}
	}

	if (!arg_toggle_isempty(PSLOW_ARG)) {
		if (arg_toggle_val(PSLOW_ARG)) 
			cfgreg_set(CFG_CONTROL, CFG_PSLOWMEM);
		else
			cfgreg_unset(CFG_CONTROL, CFG_PSLOWMEM);
	}
	
	
	if (!arg_switch_isempty(EXTRAMEM_ARG))
		{

		if(!(r1 & CFG_EXTRAMEM) && (r0 & CFG_CC_STAT)) {
			cfgreg_set(CFG_CONTROL, CFG_EXTRAMEM);
			memory_add_misc();
		}
	}



	/* Display status only if no arguments were given. */
	if (argc == 1) {
		status_display();
	}

	cfgreg_lock();

	/* Free everything and return to OS. */
	FreeArgs(result);
	FreeVec(argArray);

	return returnCode_EXIT;
}

/* reboot the machine */
void
reboot(void)
{
	/* think about opening graphics.library, 
	   then LoadView(); 2x WaitTOF(); to work around V39 bug */

	/* for now let's just call Exec's ColdReboot()... */
	ColdReboot();
}

