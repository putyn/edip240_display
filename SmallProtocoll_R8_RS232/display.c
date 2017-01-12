/*=======================================================
  =======================================================
	Modul: display.c
	Author: ELECTRONIC ASSEMBLY
	Date: Februrary 2011
	Description:
		This modul includes the most important display
		functions
  =======================================================	
  ========================================================*/
  
  //-------------- INCLUDES ---------------//
#include "main.h"


//----------- Global-Variables ----------//

//------- Modul-Global-Variables --------//

/*============================================================================
Function: 	eDIP_command6
Input:		* uchar data, uchar data1, data2, data3, data4, data5, data6, parameter
Output:		Bool (true if ACK received, false if transfer failed)
Discription:transfers simple commands, with "parameter" variables, this is an
			internal function, use definitions (macros)
============================================================================*/
_Bool eDIP_command6(unsigned char * data, unsigned char data1, unsigned char data2, unsigned char data3, unsigned char data4, unsigned char data5, unsigned char data6, unsigned char parameter)
{
	unsigned char buf[9]=0;

	buf[0]=ESC;
	buf[1]=*data++;
	buf[2]=*data;
	buf[3]=data1;
	buf[4]=data2;
	buf[5]=data3;
	buf[6]=data4;
	buf[7]=data5;
	buf[8]=data6;
	
	return send_Command(buf, parameter+3);	//3 is aggredated, because 3 bytes added additionally
}

/*============================================================================
Function: 	eDIP_string
Input:		* uchar data, uint x, uint, y, * uchar string, uchar len
Output:		Bool (true if ACK received, false if transfer failed)
Discription:outputting strings on display, this is an
			internal function, use definitions (macros)
============================================================================*/
_Bool eDIP_string(unsigned char * data, unsigned int x, unsigned int y, unsigned char * string, unsigned char length)
{
	unsigned char buf[8+MAX_STRING]=0; 	//MAX_STring is definied in display.h
										//the 8 is aggregated, because of 8 bytes that are needed
										//additionally
	unsigned char i=0;
	buf[0]=ESC;
	buf[1]=*data++;
	buf[2]=*data;
	buf[3]=x&0x00FF; 		//low byte of x-position
	buf[4]=(x>>8)&0x00FF;	//high-byte of x-position
	buf[5]=y&0x00FF;		//low-byte of y-position
	buf[6]=(y>>8)&0x00FF;	//high-byte of y-positon
	
	if(length>MAX_STRING) 	//be sure, that string is not too long and
		length=MAX_STRING;	//exeeds buffersize
	 
	for(i=0; i<length; i++) //store string
	{
		buf[7+i]=*string++;
	}
	buf[7+i]=0; //Close string
	
	return send_Command(buf, length+8);	//8 is aggredated, because 8 bytes added additionally
}

/*============================================================================
Function: 	eDIP_string2
Input:		* uchar data, * uchar string, uchar len
Output:		Bool (true if ACK received, false if transfer failed)
Discription:outputting strings on terminal, this is an
			internal function, use definitions (macros)
============================================================================*/
_Bool eDIP_string2(unsigned char * data, unsigned char * string, unsigned char length)
{
	unsigned char buf[3+MAX_STRING]=0;	//MAX_STring is definied in display.h
										//the 3 is aggregated, because of 3 bytes that are needed
										//additionally
	unsigned char i=0;
	buf[0]=ESC;
	buf[1]=*data++;
	buf[2]=*data;
	
	if(length>MAX_STRING) 	//be sure, that string is not too long and
		length=MAX_STRING;	//exeeds buffersize
	 
	for(i=0; i<length; i++)
	{
		buf[3+i]=*string++;
	}
	buf[3+i]=0; //Close string
	
	return send_Command(buf, length+3);	//3 is aggredated, because 3 bytes added additionally
}

/*============================================================================
Function: 	eDIP_draw
Input:		* uchar data, uint x1, y1, x2, y2, x3, y3, uchar parameters
Output:		Bool (true if ACK received, false if transfer failed)
Discription:drawing something on display, this is an
			internal function, use definitions (macros)
============================================================================*/
_Bool eDIP_draw(unsigned char * data, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3, unsigned char parameters)
{
	unsigned char buf[15]=0;

	buf[0]=ESC;
	buf[1]=*data++;
	buf[2]=*data;
	buf[3]=x1&0x00FF;
	buf[4]=(x1>>8)&0x00FF;
	buf[5]=y1&0x00FF;
	buf[6]=(y1>>8)&0x00FF;
	buf[7]=x2&0x00FF;
	buf[8]=(x2>>8)&0x00FF;
	buf[9]=y2&0x00FF;
	buf[10]=(y2>>8)&0x00FF;
	buf[11]=x3&0x00FF;
	buf[12]=(x3>>8)&0x00FF;
	buf[13]=y3&0x00FF;
	buf[14]=(y3>>8)&0x00FF;
	
		
	return send_Command(buf, parameters+3);	//3 is aggredated, because 3 bytes added additionally
}

/*============================================================================
Function: 	eDIP_bargraph
Input:		* uchar data, uint x1, y1, x2, y2, x3, y3, uchar nr, sv, ev, type
Output:		Bool (true if ACK received, false if transfer failed)
Discription:bargraph, this is an internal function, use definitions (macros)
============================================================================*/
_Bool eDIP_bargraph(unsigned char * data, unsigned char nr, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char sv, unsigned char ev, unsigned char type)
{
	unsigned char buf[15]=0;

	buf[0]=ESC;
	buf[1]=*data++;
	buf[2]=*data;
	buf[3]=nr;	
	buf[4]=x1&0x00FF;		//xstart low-byte
	buf[5]=(x1>>8)&0x00FF;	//xstart high-byte
	buf[6]=y1&0x00FF;		//ystart low-byte
	buf[7]=(y1>>8)&0x00FF;	//ystart high-byte
	buf[8]=x2&0x00FF;		//xend low-byte
	buf[9]=(x2>>8)&0x00FF;	//xend high-byte
	buf[10]=y2&0x00FF;		//yend low-byte
	buf[11]=(y2>>8)&0x00FF;	//yend high-byte
	buf[12]=sv;
	buf[13]=ev;
	buf[14]=type;
		
	return send_Command(buf, 15);	
}

/*============================================================================
Function: 	eDIP_touch1
Input:		* uchar data, uint x1, y1, x2, y2, * uchar string, uchar len, dc, uc
Output:		Bool (true if ACK received, false if transfer failed)
Discription:define touch area, this is an internal function, 
			use definitions (macros)
============================================================================*/
_Bool eDIP_touch1(unsigned char * data, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char dc, unsigned char uc, unsigned char * string, unsigned char length)
{
	unsigned char buf[14+MAX_STRING]=0;	//MAX_STring is definied in display.h
										//the 14 is aggregated, because of 14 bytes that are needed
										//additionally
	unsigned char i=0;
	buf[0]=ESC;
	buf[1]=*data++;
	buf[2]=*data;
	buf[3]=x1&0x00FF;		//xstart low-byte
	buf[4]=(x1>>8)&0x00FF;	//xstart high-byte
	buf[5]=y1&0x00FF;		//ystart low-byte
	buf[6]=(y1>>8)&0x00FF;	//ystart high-byte
	buf[7]=x2&0x00FF;		//xend low-byte
	buf[8]=(x2>>8)&0x00FF;	//xend high-byte
	buf[9]=y2&0x00FF;		//yend low-byte
	buf[10]=(y2>>8)&0x00FF;	//yend high-byte
	buf[11]=dc;				//downcode
	buf[12]=uc;				//upcode
	
	if(length>MAX_STRING)
	 length=MAX_STRING;
	 
	for(i=0; i<length; i++)
	{
		buf[13+i]=*string++;
	}
	buf[13+i]=0; //Close string
	
	return send_Command(buf, length+14);	
}

/*============================================================================
Function: 	eDIP_touch2
Input:		* uchar data, uint x1, y1, , * uchar string, uchar len, dc, uc
Output:		Bool (true if ACK received, false if transfer failed)
Discription:define touch area, this is an internal function, 
			use definitions (macros)
============================================================================*/
_Bool eDIP_touch2(unsigned char * data, unsigned int x1, unsigned int y1, unsigned char dc, unsigned char uc, unsigned char * string, unsigned char length)
{
	unsigned char buf[10+MAX_STRING]=0;	//MAX_STring is definied in display.h
										//the 10 is aggregated, because of 10 bytes that are needed
										//additionally
	unsigned char i=0;
	buf[0]=ESC;
	buf[1]=*data++;
	buf[2]=*data;
	buf[3]=x1&0x00FF;
	buf[4]=(x1>>8)&0x00FF;
	buf[5]=y1&0x00FF;
	buf[6]=(y1>>8)&0x00FF;
	buf[7]=dc;
	buf[8]=uc;
	
	if(length>MAX_STRING)
	 length=MAX_STRING;
	 
	for(i=0; i<length; i++)
	{
		buf[9+i]=*string++;
	}
	buf[9+i]=0; //Close string
	
	return send_Command(buf, length+10);	
}

/*============================================================================
Function: 	answerdecode
Input:		---
Output:		Bool (true if answer is handled, false if not)
Discription:chooses the right reaction on answers of display
============================================================================*/
_Bool answerdecode(void)
{
	switch(datareceive[3])
	{
		//Response from the analog touch panel when a key/switch is pressed. code = down or up code of the
		//key/switch. It is only transmitted if no touch macro with the number code is defined !
		case 'A':
				return react_A();
				break;
		//When a bargraph is set by touch, the current value of the bar no is transmitted. Transmission of the bar balue
		//must be activated (see the 'ESC A Q n1' command).
		case 'B':
		break;
		//After the input port is changed, the new 8-bit value is transmitted. The automatic port scan must be activated.
		//See the 'ESC Y A n1' command. It is only transmitted when there is no corresponding port/bit macro defined !
		case 'P':
		break;
		//When a keystroke of the external matrix keyboard is detected, the newly pressed key number no is
		//transmitted. Only transmitted if no corresponding matrix macro is defined !
		case 'M':
		break;
		//The following is transmitted in the case of a free touch area event: type=0 is release; type=1 is touch; type=2
		//is drag within the free touch area at the coordinates xx1, yy1
		case 'H':
		break;
		//After the 'ESC A X' command, the current status (value=0 or 1) of the touch switch code is transmitted.
		case 'X':
		break;
		//After the 'ESC A G nR' command, the code of the active touch switch in the radio group no is sent.
		case 'G':
		break;
		//After the 'ESC Y R' command, the requested input port is transmitted. no=0: value is an 8-bit binary value of
		//all 8 inputs. no=1..8: value is 0 or 1 depending on the status of the input no
		case 'Y':
		break;
		//After the 'ESC V D ch' command, the requested voltage of channel ch=1..2 will be sent (value = 0..5000mV)
		case 'D':
		break;	
		//After the 'ESC V S ch' command, the requested voltage of channel ch=1..2 will be set as scaled ASCII
		//characters (length of string = num-1).
		case 'W':
		break;
		//After the 'ESC S V' command, the version of the edip firmware is transmitted as a string
		//e.g. "EA eDIPTFT43-A V1.0 Rev.A TP+"
		case 'V':
			return react_V();
			break;
		//After the 'ESC S J' command, the macro-projectname is transmitted. e.g. "init / delivery state"
		case 'J':
		break;
		//after the 'ESC S I' command, internal information is sent by eDIP (16-Bit integer values LO-HI Byte)
		//Version: LO-Byte = version number Software; HI-Byte = Hardware revison letter touch
		//Touchinfo: LO-Byte = '-|+' X direction detected; HI-Byte = '-|+' Y direction detected
		//DFlen: number of user bytes in data flash memory (3 Bytes: LO-, MID- HI-Byte)
		case 'I':
		break;
		//After the 'ESC F S n1' command, the current value of the instrument with the number n1 is transmitted.
		case 'F':
		break;
		//default
		default:
			return FALSE;
			break;
	}
	return FALSE;
}

/*============================================================================
Function: 	react_A
Input:		---
Output:		Bool (true if answer is handled, false if not)
Discription:reacts on touch answers from display, depends on your project,
			please change it for your purpose
============================================================================*/
_Bool react_A(void)
{
	unsigned int i=0;
	switch(datareceive[5])
	{
		case 1: //Button 1 is pressed, requesting Version
			return eDIP_SV();
			break;
		case 2: //Button 2 is pressed, want to delete an area
			return eDIP_RL(5,205,XMAX,YMAX);
			break;
		case 3: //Button 3 is pressed, want to reset display contents
			CheckComm(eDIP_DL());
			CheckComm(eDIP_FZ(GREEN,BLACK));
			CheckComm(eDIP_ZF(SWISS30B));
			CheckComm(eDIP_ZC(XMAX/2, YMAX/2-15, "RESET", 5));
			for (i=0; i<0xFFFF;i++)
			{
				asm("nop");
			}
			samplescreen();
			return TRUE;
			break;
		default: //button reaction not definied
			return FALSE;
			break;
	}
	return FALSE;
}

/*============================================================================
Function: 	react_V
Input:		---
Output:		Bool (true if answer is handled, false if not)
Discription:reacts on send version -> sends it back on scren; 
			depends on your project, please change it for your purpose
============================================================================*/
_Bool react_V(void)
{
	
	CheckComm(eDIP_FZ(WHITE,BLACK));
	CheckComm(eDIP_ZF(GENEVA10));
	
	
	
	return eDIP_ZL(5,205,&datareceive[5],datareceive[4]);
}

//---------------------EOF---------------------//
