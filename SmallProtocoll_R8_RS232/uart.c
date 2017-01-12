/*=======================================================
  =======================================================
	Modul: uart.c
	Author: J. Moertl
	Date: Februrary 2011
	Description:
		This modul includes serial interface to eDIP=Series.
		The serial interface is set: 9600Bd, no parity, 1 Stopbit, LSB first
		SmallProtocoll is implemented.
  =======================================================	
  ========================================================*/


//-------------- INCLUDES ---------------//
#include "main.h"


//----------- Global-Variables ----------//
unsigned char datareceive[DATARECEIVEBUF]=0;		//received data (stored only if bcc is correct, without ACK-Bytes)
unsigned char failurecounter=0; 					//if this variable is 0xFF there is a communication failure
													//if it is 0xFE, then data is present

//------- Modul-Global-Variables --------//
unsigned char receive[RECEIVEBUF]=0;	//receive-array
unsigned char receive_content=0; 		//positon of next free space in receive-array
unsigned char receive_interpretation=0; //interpreted data of receive-array

unsigned char transmit[TRANSMITBUF]=0;	//transmit-array
unsigned char transmitsendstart=0;		//position of data to send in transmit-array
unsigned char transmitsendend=0;		//position of last data in transmit-array
unsigned char transmitbusy=FALSE;		//true, if data is present in transmit-array
unsigned char transmitpackage=FALSE;	//true, if datapackage is send


unsigned char dataptr=0;				//postion of next free space in datareceive-array





/*============================================================================
Function: 	initUart
Input:		---
Output:		---
Discription:Initialises serial Interface with 9600Bd, 1 Stop, no parity,
			LSB first, RxInterrupt (Lvl 2), TxCompleteInterrupt (Lvl 1)
============================================================================*/
void initUart(void)
{
	pd1_4=1; 	//Define Port as output (Txd)
	p14sel0=1; 	//Use Port 1_4 as Txd
	p14sel1=0;
	p14sel2=0;
	
	pd1_5=0; 	//Define Port as input (Rxd)
	p15sel0=1; 	//Use Port 1_5 as Rxd
	p15sel1=0;
	p15sel2=0;
	
	
	u0mr = 0x05; //8Bit, 1 stop, internal clock, no parity
	u0c0 = 0x08; //prescaler 1, LSB first
		
	u0brg = 129; //9600Baud (20MHz/(16*9600))-1=129,21
//	u0brg = 64;  //19200Baud (20MHz/(16*19200))-1=64,10
//	u0brg = 10;  //115200Baud (20MHz/(16*115200))-1=9,85	


	u0c1 = 0x17; //transmission, receive enable, continous receive disabled
	u0ir = 0x0C; //Interrupt enable (Rx and Tx)
	
	
	ilvl8 = 0x30; //Rx interrupt level 3
	ilvl9 = 0x02; //Tx complete level 2
	
}

/*============================================================================
Function: 	send_Command
Input:		uchar * data, uchar length
Output:		Bool (true if ACK received, false if transfer failed
Discription:send commands and data to eDIP with SmallProtocoll
============================================================================*/
_Bool send_Command (unsigned char * data, unsigned char length)
{
	unsigned char failure=0;
	uart_send_array_smallprot(data, length, FALSE); //Send data
	while(transmitpackage)	//wait until last package was correctly send (wait for Ack from display)
	{	
		failure=uart_receive_decode();  //check for Ack-Byte
	}
	//RETRY is set by user as definition in headerfile
	if (failure>RETRY)
		return FALSE;
	else
		return TRUE;
}

/*============================================================================
Function: 	request_Buffer_Content
Input:		---
Output:		Bool (true if ACK received, false if transfer failed
Discription:requestest Buffer content
============================================================================*/
_Bool request_Buffer_Content (void)
{
	unsigned char failure=0;
	
	uart_send_array_smallprot("S",1,TRUE); //request buffer content

	while(transmitpackage)	//wait until last package was correctly send (wait for Ack from display)
	{	
		failure=uart_receive_decode();  //check for Ack-Byte
	}
	if (failure==0xFF) //if no ACK return leave functrion with failure
		return FALSE;
	while( failure != 0xFE) //receive all data from display
	{
		failure=uart_receive_decode();
		if(timer_ms>TIMEOUTRX)	//Timeout between bytes?
		{
			while(timer_ms<=TIMEOUT) //wait timeouttime of display
			{
			} 
			timer_ms=0;
			repeat_Package();		//resend package
			failure++;
			
			if (failure>RETRY)		//if tried RETRY-times, then failure
				return FALSE;	
		}
	}
	//If everything is ok it is returned 0xFE by uart receive decode
	if (failure==0xFE)
		return TRUE;
	else
		return FALSE;
}

/*============================================================================
Function: 	request_Buffer_Info
Input:		---
Output:		Bool (true if ACK received, false if transfer failed
Discription:requests Buffer-Info
============================================================================*/
_Bool request_Buffer_Info (void)
{
	unsigned char failure=0;
	
	uart_send_array_smallprot("I",1,TRUE);
	
	for (failure=0; failure<255; failure++){}
	failure=0;
	
	while(transmitpackage)	//wait until last package was correctly send (wait for Ack from display)
	{	
		failure=uart_receive_decode();  //check for Ack-Byte
	}
	if (failure==0xFF)
		return FALSE;
	while( failure != 0xFE)
	{
		failure=uart_receive_decode();
		if(timer_ms>TIMEOUTRX)
		{
			while(timer_ms<=TIMEOUT) //wait timeouttime
			{
			} 
			timer_ms=0;
			repeat_Package();
			failure++;
			
			if (failure>RETRY)
				return FALSE;	
		}
	}
	if (failure==0xFF)
		return FALSE;
	else
		return TRUE;
}

/*============================================================================
Function: 	set_Protocollsettings
Input:		uchar packet, uchar timeout
Output:		Bool (true if ACK received, false if transfer failed
Discription:sets protocollsettings of SmallProtocoll
============================================================================*/
_Bool set_Protocollsettings (unsigned char packet, unsigned char timeout)
{
	unsigned char failure=0;
	unsigned char buf[3]={'D',packet,timeout};
	uart_send_array_smallprot(buf,3,TRUE);

	while(transmitpackage)	//wait until last package was correctly send (wait for Ack from display)
	{	
		failure=uart_receive_decode();  //check for Ack-Byte
	}
	if (failure==0xFF)
		return FALSE;
	else
		return TRUE;
}

/*============================================================================
Function: 	request_Protocollsettings
Input:		---
Output:		Bool (true if ACK received, false if transfer failed
Discription:requests Protocollsettings from display and stores them in datareceivebuffer[].
============================================================================*/
_Bool request_Protocollsettings (void)
{
	unsigned char failure=0;
	uart_send_array_smallprot("P",1,TRUE);
	
	while(transmitpackage)	//wait until last package was correctly send (wait for Ack from display)
	{	
		failure=uart_receive_decode();  //check for Ack-Byte
	}
	if (failure==0xFF)
		return FALSE;
	while(failure != 0xFE)
	{
		failure=uart_receive_decode();
		if(timer_ms>TIMEOUTRX)
		{
			while(timer_ms<=TIMEOUT) //wait timeouttime
			{
			} 
			timer_ms=0;
			repeat_Package();
			failure++;
			
			if (failure>RETRY)
				return FALSE;	
		}
	}
	if (failure==0xFE)
		return TRUE;
	else
		return FALSE;
}

/*============================================================================
Function: 	repeat_Package
Input:		---
Output:		Bool (true if ACK received, false if transfer failed
Discription:requests last sended package from display to send again
============================================================================*/
_Bool repeat_Package (void)
{
	unsigned char failure=0;
	uart_send_array_smallprot("R",1,TRUE);
	
	while(transmitpackage)	//wait until last package was correctly send (wait for Ack from display)
	{	
		failure=uart_receive_decode();  //check for Ack-Byte
	}
	if (failure==0xFF)
		return FALSE;
	else
		return TRUE;
}

/*============================================================================
Function: 	set_Adress485
Input:		Bool select, uchar Adress
Output:		Bool (true if ACK received, false if transfer failed
Discription:calls RS485 adress and selects/deselcts it
============================================================================*/
_Bool set_Adress485 (_Bool select, unsigned char Adress)
{
	unsigned char failure=0;
	unsigned char buf[3]={'A','D',Adress};
	if(select)
		buf[1]='S';

	uart_send_array_smallprot(buf,3,TRUE);
	
	while(transmitpackage)	//wait until last package was correctly send (wait for Ack from display)
	{	
		failure=uart_receive_decode();  //check for Ack-Byte
	}
	if (failure==0xFF)
		return FALSE;
	else
		return TRUE;
}




/*============================================================================
Function: 	uart_send
Input:		uchar data
Output:		---
Discription:puts data into tx-register, if uart busy, 
			then data in transmit-buffer
============================================================================*/
static void uart_send(unsigned char data)
{
	if (transmitbusy==FALSE)  //Tx idle
	{
		transmit[transmitsendend]=data; 	//store data in tx buffer
											//important, if you have to resend packages
										
		transmitsendend++;					//added one byte to tx-buffer
		transmitsendstart++;				//sended one byte (2. lines bellow)
		transmitbusy=TRUE;					//tx is operating
		u0tb=data;							//send data
	}
	else	//Tx busy -> stoe data and send later
	{
		transmit[transmitsendend]=data;  //store data in tx buffer
		transmitsendend++;				 //added one byte to tx-buffer
	}
}

/*============================================================================
Function: 	uart_send_array
Input:		uchar * data, uchar length
Output:		---
Discription:sends array, using uart_send(), length = dataamount to be send
============================================================================*/
static void uart_send_array(unsigned char * data, unsigned char length)
{
	unsigned char i=0;  //internal index variable
	
	for( i=0; i<length; i++)
	{
		uart_send(*data++); //send data and increment readpointer for next step
	}
}

/*============================================================================
Function: 	uart_send_array_smallprot
Input:		uchar * data, uchar length, uchar ptotocoll
Output:		---
Discription:Sends data to display, adding protocoll:
			DC1/DC2 | length | data..... | bcc
			<DC1> = 17(dec.) = 0x11 <DC2> = 18(dec.) = 0x12 
			length = count of user data (without <DC1/DC2>, without checksum bcc)
			bcc = 1 byte = sum of all bytes incl. <DC1/DC2> and len, modulo 256
			sends array, using uart_send(), length = data amount to be send
			protocoll = False (Command/Data to the display)
			protocoll = True (protocoll commands to the display)
============================================================================*/
static void uart_send_array_smallprot(unsigned char * data, unsigned char length, unsigned char protocoll)
{
	unsigned char i,bcc=0;	//internal index variable and checksum
	
	while(transmitpackage)	//wait until last package was correctly send (wait for Ack from display)
	{	
		uart_receive_decode();  //check for Ack-Byte
	}
	
	transmitpackage=length;		//save length of datapackage
	
	if(protocoll)				//protocoll commands to display
	{
		uart_send(0x12);		//send DC2
		bcc = 0x12;				
	}
	else						//Command/Data to the display
	{
		uart_send(0x11);		//send DC1
		bcc = 0x11;
	}
	
	uart_send(length); 			//send data length
	bcc = bcc + length;
	
	for( i=0; i<length; i++)	
	{
		uart_send(*data);		//send data 
		bcc = bcc + (*data++);  //add to checksum and increment readpointer for next step
	}
	
	uart_send(bcc);				//send checksum
	timer_ms=0;					//timer to 0, to check timeout of display answer
	
}

/*============================================================================
Function: 	uart_receive_decode
Input:		---
Output:		---
Discription:decode received data, check if display answered correctly
============================================================================*/
static unsigned char uart_receive_decode(void)
{
	unsigned char data=0x15; //received data -> preset with failure (NAK)
	unsigned char timeoutwait=TRUE; //internal variable to wait timeout
	
	if(receive_interpretation<receive_content) //new data present
	{
		data=receive[receive_interpretation];  //get new data
		receive_interpretation++; //decoded one byte, increment
	
		if(transmitpackage)	//waiting for answer of display (NAK or AK - Byte)
		{
			if(data==0x06) //ACK received -> ok
			{
				transmitpackage=FALSE; //package correctly send
				transmitsendstart=transmitsendend=0; //can use buffer from beginning
				failurecounter=0; //failure to 0
			}
			else		//received not ACK
			{
				if(data==0x15) //received NAK -> retry package at once
				{
					if(failurecounter<RETRY) //failurecounter send package is under maximum retry
					{
						//resend package, it is still stored in tx buffer, length of last package is stored in
						//transmitpackage added with 3 for DC, length and bcc
						uart_send_array(&transmit[transmitsendend-(transmitpackage+3)], transmitpackage+3);
						failurecounter++; //increment resend package
						timer_ms=0; //timeout timer to 0
					}
					else
						failurecounter=0xFF; //failurecounter send package is maximum -> set to failure
				}
				else //received anything else (no ACK or NACK)
				{
					if(failurecounter<RETRY) //failurecounter send package is under maximum retry
					{
						timer_ms=0;//reset timer for timeouttime
						while(timeoutwait) //wait timeouttime
						{
							if (timer_ms>TIMEOUT)
								timeoutwait=FALSE;
						} 
						//resend package, it is still stored in tx buffer, length of last package is stored in
						//transmitpackage added with 3 for DC, length and bcc
						uart_send_array(&transmit[transmitsendend-(transmitpackage+3)], transmitpackage+3);
						failurecounter++; //increment resend package
						timer_ms=0; //timeout timer to 0
					}
					else
						failurecounter=0xFF; //failurecounter send package is maximum -> set to failure
				}
			}
		}
		else //real data
		{
			timer_ms=0;
			datareceive[dataptr]=data;
			dataptr++;
			if (dataptr>datareceive[1]+2)
			{
				if(decodeData(datareceive[1]+2))
				{
					dataptr=0;
					failurecounter=0xFE;
				}
				else
				{
					if(failurecounter<RETRY) //failurecounter send package is under maximum retry
					{
						repeat_Package();
						failurecounter++;
					}
				}
			}
		}
		
		if(receive_content==receive_interpretation) //all bytes decoded? set to 0
		{
			receive_content=0;
			receive_interpretation=0; //no data present in rx buf -> reset
		}
	}
	
	if(transmitpackage) //sended package, but not received anything
	{
		if(timer_ms>TIMEOUTRX) //Is timeouttime for answer from display expired?
		{
			if(failurecounter<RETRY) //failurecounter send package is under maximum retry
			{
				timer_ms=0;//reset timer for timeouttime
				while(timeoutwait) //wait timeouttime
				{
					if (timer_ms>TIMEOUT)
						timeoutwait=FALSE;
				} 
				
				//resend package, it is still stored in tx buffer, length of last package is stored in
				//transmitpackage added with 3 for DC, length and bcc
				uart_send_array(&transmit[transmitsendend-(transmitpackage+3)], transmitpackage+3);
				failurecounter++; //increment resend package
				timer_ms=0; //timeout timer to 0
			}
			else
				failurecounter=0xFF; //failurecounter send package is maximum -> set to failure
		}
	}
	
	if (failurecounter == 0xFF)
	{
		transmitpackage=FALSE;
	}
	
	return failurecounter;
}

/*============================================================================
Function: 	decodeData
Input:		---
Output:		---
Discription:checks if received data is okk (bcc-ckeck)
============================================================================*/
static _Bool decodeData (unsigned char length)
{
	unsigned char bcc=0,i=0;
	for(i=0; i<length; i++)
	{
		bcc+=datareceive[i];

	}	
	if(bcc==datareceive[i])
		return TRUE;
	else
		return FALSE;
}

/*============================================================================
Function: 	checkbuf
Input:		---
Output:		true if data available, 2 if no data is present, FAlSE if commincation error
Discription:checks if data is availabele in sendbuffer and calls it
============================================================================*/
unsigned char checkbuf (void)
{
/*	if(request_Buffer_Info())
	{
		if(datareceive[2]>0)
		{
			return request_Buffer_Content();
		}
		else
			return 2;
	}
	else
		return FALSE;
*/
	if(request_Buffer_Content())
		return TRUE;	
	else
		return FALSE;
}

/*============================================================================
Function: 	_uart0_transmit
Input:		---
Output:		---
Discription:transmit interrupt, called if data is send
============================================================================*/
void _uart0_transmit(void)
{
	u0tif=0; //clear interruptflag
	
	if (transmitsendstart<transmitsendend) //data presend in tx buffer
	{
		u0tb=transmit[transmitsendstart]; //transmit next data
		transmitsendstart++; //increment sendpointer
		
		timer_ms=0;	//reset timer, because package send is not completed
	}
	else //no data present in tx buffer
	{
		transmitbusy=FALSE; //tx is idle
	}
}

/*============================================================================
Function: 	_uart0_receive
Input:		---
Output:		---
Discription:transmit interrupt, called if data is received
============================================================================*/
void _uart0_receive(void)
{	
	u0rif=0;	//clear interruptflag

	receive[receive_content]=u0rb; //store received data
	receive_content++; //increment rx content
}

//---------------------EOF---------------------//