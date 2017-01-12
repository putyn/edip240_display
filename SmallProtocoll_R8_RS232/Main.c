/*=======================================================
  =======================================================
	Modul: main.c
	Author: J. Moertl
	Date: Februrary 2011
	Description:
		This file includes the main program and hw-init.
  =======================================================	
  ========================================================*/
//-------------- INCLUDES ---------------//
#include "main.h"

//----------- Global-Variables ----------//


//------- Modul-Global-Variables --------//
unsigned char testarray[10]=0;

/*============================================================================
Function: 	main
Input:		---
Output:		---
Discription:main-function
============================================================================*/
void main (void)
{

	hwsetup();		//init hardware
	asm("FSET I"); 	//global interrupt enable
	

	
	samplescreen(); //show a sample screen
	
	//------------start main-loop----------------
	while (1)
	{
		
		if(check_display_buf>CHECKDISPLAY) //check if display has something in its sendbuffer
		{
			check_display_buf=0;		//reset counter to recall every CHECKDISPLAY mseconds
			if(checkbuf())			//checkbuffer
			{
					if(answerdecode())
					{
						CheckComm(1); //Display answers right
						datareceive[3]=0; //Delete last message information
					}
					else
						CheckComm(0); //Display not answering	
					
			}
			else
				CheckComm(0); //Display not answering
			
		}
	}
	//------------end main-loop----------------
}

/*============================================================================
Function: 	samplescreen
Input:		---
Output:		---
Discription:shows a small demoscreen: headline, bargraph by touch  
			controlling backlight and 3 simple buttons
============================================================================*/
void samplescreen (void)
{
		//Header

	CheckComm(eDIP_TA());

	CheckComm(eDIP_FD(WHITE,BLACK));
	CheckComm(eDIP_DL());
	CheckComm(eDIP_FZ(RED,BLACK));
	CheckComm(eDIP_ZF(SWISS30B));
	CheckComm(eDIP_ZC(XMAX/2, 30/4+5, "eDIP-Demo RS232|with SmallProtocoll", 35));
	CheckComm(eDIP_FG(WHITE,BLACK));
	CheckComm(eDIP_GZ(2,2));
	CheckComm(eDIP_GD(0,30*2+7,XMAX,30*2+7));
	//Brightness
	CheckComm(eDIP_AQ(0));
	CheckComm(eDIP_FZ(WHITE,BLACK));
	CheckComm(eDIP_ZF(CHICAGO14));
	CheckComm(eDIP_ZL(5, 30*2+15, "Adjust brightness:|by bargraph", 30));
	CheckComm(eDIP_FB(GREEN,RED,WHITE));
	CheckComm(eDIP_BM(13));
	CheckComm(eDIP_BR(1,20,110,XMAX-20,140,5,100,1));
	CheckComm(eDIP_YH(80));
	CheckComm(eDIP_BA(1,80));
	CheckComm(eDIP_AB(1));
	CheckComm(eDIP_YB(1));
	//define Touchbuttons
	CheckComm(eDIP_FE(BLUE,WHITE,BLUE,YELLOW,BLUE,YELLOW));
	CheckComm(eDIP_AF(CHICAGO14));
	CheckComm(eDIP_FA(WHITE,BLUE));
	CheckComm(eDIP_AT(5,160,140,190,0,1,"CShow Version",13)); //Button to show version as string on screen
	CheckComm(eDIP_AT(150,160,285,190,0,2,"CDelete",13));	  //Button to delete area, where Versionstring is shown
	CheckComm(eDIP_AT(XMAX-100,72,XMAX,100,0,3,"CReset",13)); //Button to rebuild scrren
}

/*============================================================================
Function: 	hwsetup
Input:		---
Output:		---
Discription:inits hardware
============================================================================*/
void hwsetup (void)
{
	asm("FCLR I"); //global interrupt disable
	
	ConfigureOperatingFrequency(); //set right clock
	initTimer_rb(); //timer
	initUart();		//uart
	//Portpins
	pd3_7 = 1; 		//LED-Pin as output
	ERRORLED = LED_OFF; //Error-LED off

}

/*============================================================================
Function: 	ConfigureOperatingFrequency
Input:		---
Output:		---
Discription:inits clock to external 20MHz without any divisors
============================================================================*/
void ConfigureOperatingFrequency (void)
{
	// Variable defined to stablize the oscillator
	unsigned char osc_stab = 128;

	// Disable interrupts
	asm("FCLR I");

	// Protection off
	prc0 = 1;

	// Pin P4_6 and P4_7 are configured as XIN and XOUT
	ckpt0 = 1;
	ckpt1 = 1;

	// XIN clock oscillates
	hscksel = 0;

	// System Base claock slect bit
	scksel = 1;

	// System base clock division = No division
	// System clock = System base clock
	// fAD = System base clock
	phisel = 0x00;

	// System clock division = No division
	// CPU clock = System clock
	phissel0 = 0;
	phissel1 = 0;
	phissel2 = 0;

	// Wait till the oscillator stabilizes
	while (osc_stab)
	{
		--osc_stab;
	}

	// Protection on
	prc0 = 0;
}

//---------------------EOF---------------------//