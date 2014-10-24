/*****************************************************************
 * Driver for ACM0802C LCD Module (8 characters by 2 lines )
 * on the Renesas SKP boards - header file
 *
 *  Copyright (c) 2003 Renesas Technology America, Inc.
 *  All rights reserved.
 *  v0.4 1/05/2004
 *****************************************************************/
#include "skp_bsp.h"


#ifndef _SKP_LCD_H
#define _SKP_LCD_H


#define BACKLIGHT_PIN		p9_3
#define BACKLIGHT_PIN_DDR	pd9_3
#define BACKLIGHT_PIN_MASK	0x08

/**********************************************************************************/
/* External Application Interface												  */
/**********************************************************************************/
extern BYTE x_address;
extern BYTE y_address;
BYTE Get_x_address();
BYTE Get_y_address();
BYTE Set_x_address(BYTE new_x);		// [0..7] Row number
BYTE Set_y_address(BYTE new_y);		// [0..128] "column" ad
BYTE Increment_y_address();
BYTE Increment_x_address();
void SendAddresses();  				// Send to display x and y address

#define MAX_Y 128
#define MAX_X 7

/**********************************************************************************/
/* External Application Interface												  */
/**********************************************************************************/
void display_init( void );
void TransferBuffer(_far char * buffer);
void TransferBufferFixedLength( _far const char * string, BYTE length );
void GotoAddress(char mLine, char mColumn);
void WaitForNotBusy(char controller);
char Busy(char controller);
void LCD_write(unsigned char data_or_ctrl, _far unsigned char value, char controller=1);
unsigned char LCD_read(unsigned char data_or_ctrl, unsigned char controller);
void DisplayDelay(unsigned long int units);
void DrawAllOn();
//void Clear_EOL();
void DrawTach(U8 origy, U8 origx, U8 msetting);
void DrawIcon(U8 origy, U8 origx, _far struct stIcon* mIcon );


/**********************************************************************************/
/* HARDWARE INTERFACE DEFINITIONS
/**********************************************************************************/
/*
*/
#define DATA 1
#define CTRL 0



/**********************************************************************************/
// LCD commands - use disp_ctrlw function to write these commands to the LCD. 
/**********************************************************************************/
#define DISPLAY_ON  	0x3F	// 
#define DISPLAY_OFF 	0x3E	// 
#define SET_Y_ADDRESS	0x40	// Plus the 5 bit address
#define SET_X_ADDRESS	0xB8	// Plus the 3 bit page
#define SET_DISPLAY_START		0xC0	// Plus the 5 bit page

#define BOTH   3				// Both controllers param


#endif

/**********************************************************************************/
/* NOT USED YET 																  */
/**********************************************************************************/
/*#define MAXIMUM_LINES		8		// number of lines on the LCD display
#define NUMB_CHARS_PER_LINE	20		// Maximum charactors per line of LCD display.  
#define LCD_LINE1 0
#define LCD_LINE2 16 */