/*=======================================================
  =======================================================
	Modul: timer .c
	Author: ELECTRONIC ASSEMBLY
	Date: Februrary 2011
	Description:
		This modul inits timer to 1ms
		This is the header file
  =======================================================	
  ========================================================*/
#ifndef _TIMER_H
#define _TIMER_H

//---------------Global Variables---------------//
extern unsigned int timer_ms;			//timer for 1ms
extern unsigned int check_display_buf; //timercounter to check for displaybuffer

//------------------Functions-------------------//
void initTimer_rb (void);



//-------------Interrupt-Functions--------------//
#pragma interrupt	_timer_rb2(vect=24)
void _timer_rb2(void);


#endif

//---------------------EOF---------------------//