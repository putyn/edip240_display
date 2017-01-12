/*-------------------------------------------------------
  -------------------------------------------------------
	Modul: uart.h
	Author: ELECTRONIC ASSEMBLY
	Date: Februrary 2011
	Description:
		This modul includes serial interface to eDIP-Series.
		The serial interface is set: 9600Bd, no parity, 1 Stopbit, LSB first
		SmallProtocoll is implemented.
		This is the headerfile
  -------------------------------------------------------	
  --------------------------------------------------------*/
#ifndef _UART_H
#define _UART_H

#define RECEIVEBUF 64   	//size of rx buffer
#define DATARECEIVEBUF 64 	//size of data rx buffer
#define TRANSMITBUF 32	 	//tx buffer
#define TIMEOUTRX 10		//timeout time for answer
#define TIMEOUT 250			//timeout time if no answer
#define RETRY 3				//maximum retry sending packages
#define CHECKDISPLAY 100	//time to poll for sendbufferbytes (evry x ms)

//---------------Global Variables---------------//
extern unsigned char datareceive[DATARECEIVEBUF]; //array where received data is stored
extern unsigned char failurecounter; //counts corrupt packages

//------------------Functions-------------------//
//Use this macro to be sure sending data is correct
//if failure, LED is switched on, replace with your
//errorhandling.
//failurecounter to 0 means, that communication starts again, if display is
//answering anew.
#define CheckComm(var1)		if(!var1){ERRORLED=LED_ON;failurecounter=0;}else{ERRORLED=LED_OFF;}

void initUart						(void); //Init
//do not use this functions, use functions of display.h instead.
_Bool send_Command					(unsigned char * data, unsigned char length);
_Bool request_Buffer_Content		(void);
_Bool request_Buffer_Info			(void);
_Bool set_Protocollsettings			(unsigned char packet, unsigned char timeout);
_Bool request_Protocollsettings		(void);
_Bool repeat_Package				(void);
_Bool set_Adress485					(_Bool select, unsigned char Adress);
unsigned char checkbuf 				(void);



//basic communication functions
static void uart_send_array_smallprot	(unsigned char * data, unsigned char length,unsigned char protocoll);
static unsigned char uart_receive_decode(void);
static _Bool decodeData 				(unsigned char length);

static void uart_send					(unsigned char data);
static void uart_send_array				(unsigned char * data, unsigned char length);


//-------------interrupt Functions---------------//
#pragma interrupt	_uart0_transmit(vect=17)
void _uart0_transmit(void);
#pragma interrupt	_uart0_receive(vect=18)
void _uart0_receive(void);

#endif

//---------------------EOF---------------------//