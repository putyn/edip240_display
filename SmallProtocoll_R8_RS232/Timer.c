/*=======================================================
  =======================================================
	Modul: timer .c
	Author: J. Moertl
	Date: Februrary 2011
	Description:
		This modul inits timer to 1ms
  =======================================================	
  ========================================================*/

//----------------INCLUDES---------------//
#include "main.h"

//------------ Global-Variables ---------//
unsigned int timer_ms=0;		  //timercounter  (1 ms)
unsigned int check_display_buf=0; //timercounter to check for displaybuffer

//-------- Modul-Global-Variables--------//



/*============================================================================
Function: 	initTimerRB
Input:		---
Output:		---
Discription:Initialises Timer RB to 1ms
============================================================================*/
void initTimer_rb(void)
{		
	ilvlc &= 0xFC;										//interrupt disable
	mstcr &= 0x71;										//Timer RB2 Active
	tstart_trbcr = 0;									//Count Stop

	while(tcstf_trbcr == 1);							// Wait timer to stop

	trbcr = 0x04;										//Stop count at once, to init
														//TRBPRE and TRBPR Register
														//START and TCSTF bits of TRBOCR Register can be set, too
	
	trbir = 0x00;										//Timer RB2 interrupt disable

	trbmr = 0x30;										//TMOD0-TMOD1: Set to "00b" in Timer mode
														//8bit timer selected
														//Write to reload register and counter
														//"f2" in Count Source
														//Provides count source

	//Set Timer to 1ms: Period between overflow: 50ns * 200 * 2 * 50 = 1ms
	trbpre = 200-1;										//Set (200 - 1) in TRBPRE register
	trbpr = 50-1;										//Set (50 - 1) in TRBPR register
	asm("nop");
	asm("nop");

	trbif_trbir = 0;									//interrupt request clear
	trbie_trbir = 1;									//Timer RB2 interrupt enabled
	ilvlc |= 0x01;										//interrupt enable (Level 1)

	tstart_trbcr = 1;									//Start Timer RB2 operation
	while(tcstf_trbcr == 0);							//Timer RB2 count start waiting
}


/*============================================================================
Function: 	_timer_rb2
Input:		---
Output:		---
Discription:timer RB interrupt, called if 1ms is up
============================================================================*/
void _timer_rb2(void)
{
	trbif_trbir = 0; //clear interrupt flag

	timer_ms++; //increment timer counter
	check_display_buf++; //increment display check counter
}

//---------------------EOF---------------------//