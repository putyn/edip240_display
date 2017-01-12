/*=======================================================
  =======================================================
	Modul: main.c
	Author: ELECTRONIC ASSEMBLY
	Date: Februrary 2011
	Description:
		This file includes the main program and hw-init.
		This is the headerfile
  =======================================================	
  ========================================================*/
#ifndef MAIN_H
#define MAIN_H

#include "sfr_r8m12a.h" //controller definitions
#include "timer.h"
#include "uart.h"
#include "display.h"

//Some general definitions
#define ERRORLED 	p3_7
#define LED_OFF 1
#define LED_ON	0
#define TRUE	1
#define FALSE	0


//------------------Functions-------------------//
void main			(void);
void samplescreen 	(void); //a simple screen: show headline, bargraph by touch  
							//controlling backlight and 3 simple buttons
void hwsetup		(void); //controller hardware setup
void ConfigureOperatingFrequency (void); //set correct frequency of uC

#endif

//---------------------EOF---------------------//