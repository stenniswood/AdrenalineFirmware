/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Graphic LCD 128x64 Low level Display routines.	 *
 *				   Routines which directly communicate to the hardware	*
 *											   						 *
 *	  CATEGORY:   BIOS Driver										 *
 *											   						 *
 *    PURPOSE: 														 *
 *			a) To define port io data directions for read/write 	 *
 *			   operations.			 								 *
 *			b) To perform the parallel databus control/handshaking	 *
 *			c) Transfer a set of data to the LCD					 *
 *			d) Define cursor coordinates:  x for lines,			 	 *
 *				y for columns.										 *
 *																	 *
 *	  NOTE:  X axis goes down the vertical of the display.			 *
 *			 Y axis goes across the horizontal 						 *
 *			 This may be counter intuitive to the normal Euclidean 	 *
 *			 way of thinking. 										 *
 *																	 *
 *																	 *
 *	  PIN ASSIGNMENTS:												 *
 *				See "Proc_io_Defs.h"								 *
 *																	 *
 *    AUTHOR: Stephen Tenniswood, Software Engineer 	 			 *
 *			  May 7, 2007						 				 	 *
 *********************************************************************/
#include "sfr29.h"
#include "skp_bsp.h"
#include "LCD_icons.h"
#include "LCD_Driver.h"


/**********************************************************************************/
/* HARDWARE INTERFACE DEFINITIONS
/**********************************************************************************/
#define DATA_PORT_0			 p1			// Data bus port
#define PORT_DDR_0			 pd1		// Data bus direction register
#define PORT_DDR_0_DRIVE	 0xFF		// Can't read DDR reg, so have can't OR values
#define PORT_DDR_0_RECEIVE 	 0x00		// 

#define DRIVE 1							// Define OUTPUT direction
#define HIGH 1							// Logic HIGH
#define LOW 0

#define DI_PIN			p0_3			// Data or Instruction select
#define DI_PIN_MASK		0x08			// bit mask from entire port
#define DI_PIN_DDR		pd0_3			// EN pin DDR


#define RESET_PIN		p0_5		// 
#define RESET_PIN_MASK	0x20		// 
#define RESET_PIN_DDR	pd0_5		// 
#define ASSERT 0
#define UNASSERTED 1

#define EN_PIN			p0_2  		// Display Enable pin
#define EN_PIN_MASK		0x04		// bit mask from entire port
#define EN_PIN_DDR		pd0_2		// EN pin DDR
#define ENABLED  1					//
#define DISABLED 0

#define CS1_PIN			p0_7		// 
#define CS1_PIN_MASK	0x80		// 
#define CS1_PIN_DDR		pd0_7		// 

#define CS2_PIN			p0_6		// 
#define CS2_PIN_MASK	0x40		// 
#define CS2_PIN_DDR		pd0_6		// 
#define SELECTED		1
#define UNSELECTED		0

#define RW_PIN 			p0_4  		// RS Register Select pin
#define RW_PIN_MASK		0x10		// bit mask from entire port
#define RW_PIN_DDR		pd0_4		// RS pin DDR
#define READ 1
#define WRITE  0

/**********************************************************************************/
/* END HARDWARE INTERFACE DEFINITIONS
/**********************************************************************************/


// VARIABLES:
BYTE x_address = 0;
BYTE y_address = 0;
BYTE Active_Controller = 1;


/*************    ACCESSOR FUNCTIONS    ****************************/

/******************************************
 *  Y is the length	is [0..127]
 *  X is the height is [0..8]
 ******************************************/
/****************************************************************
*  Following accessor functions used as external interface only; 
*  functions internal to this file handle on their own - smartly  
*****************************************************************/
BYTE Get_x_address() { return x_address; };
BYTE Get_y_address() { return y_address; };
BYTE Set_x_address(BYTE new_x) 
{
	x_address = new_x;
	if (x_address > 7) x_address = 7;
};
BYTE Set_y_address(BYTE new_y)
{
	y_address = new_y;
	if (y_address > 127) y_address=127;
	if (y_address > 63) 
		Active_Controller = 2;
	else 
		Active_Controller = 1;
};
BYTE Increment_y_address()
{
	y_address++;
	if (y_address > 63) 
		Active_Controller = 2;
}
BYTE Increment_x_address()
{
	x_address++;
}
/*****************************************************************************
Name:          	SendAddresses()
Parameters:     First call Set_y_address() or Set_x_address()
Description:    Send the internal x,y position counters to the display
				Ensures code is in-sync with the driver.
*****************************************************************************/
void LCD_SendAddresses()   		// Send to display
{
	GotoAddress(x_address,y_address);
}

/*************    HARDWARE  RELATED  FUNCTIONS    ****************************/

/*****************************************************************************
Name:          	Port_Drive
Parameters:     none
Returns:        none
Description:    Initial data port direction to outputs
*****************************************************************************/
inline void LCD_Port_Drive()	
{	
	PORT_DDR_0  |= PORT_DDR_0_DRIVE;		// p10 outputs [0..5]
}
/*****************************************************************************
Name:          	LCD_Port_Read
Parameters:     none
Returns:        none
Description:    Initial data port direction to outputs
*****************************************************************************/
inline void LCD_Port_Read()
{
	PORT_DDR_0  &= PORT_DDR_0_RECEIVE;		// outputs
}

/*****************************************************************************
Name:          	InitDisplay
Parameters:     none
Returns:        none
Description:    Intializes the LCD display. 
*****************************************************************************/
void LCD_display_init( void )
{
	// initial port directions
	LCD_Port_Drive();
		
	CS1_PIN		= SELECTED;			// Preset Values
	CS2_PIN		= SELECTED;
	EN_PIN 		= HIGH;
	RW_PIN		= WRITE;
	DI_PIN 		= HIGH;
	BACKLIGHT_PIN = LOW;
	
	CS1_PIN_DDR |= DRIVE;			// Data Direction Control lines.
	CS2_PIN_DDR |= DRIVE;
	EN_PIN_DDR	|= DRIVE;
	RW_PIN_DDR	|= DRIVE;
	DI_PIN_DDR	|= DRIVE;
	RESET_PIN_DDR |= DRIVE;
	prc2=1;							// unprotect as Port 9 is used	
	BACKLIGHT_PIN_DDR = OUTPUT;
	
	// RESET LCD
	EN_PIN 		= LOW;
	RESET_PIN	= ASSERT;
	DisplayDelay(4);	
	RESET_PIN	= UNASSERTED;
	DisplayDelay(4);	
	WaitForNotBusy(1);

	// INITIALIZE REGISTERS:
	GotoAddress( 0,0 );				// x=0; y=0;
	LCD_write(CTRL,SET_DISPLAY_START+0x00, BOTH );
	WaitForNotBusy(1);
	LCD_write(CTRL,DISPLAY_ON    ,BOTH);
	
	BACKLIGHT_PIN = LOW;	
//	BACKLIGHT_PIN = HIGH;	
}


/*****************************************************************************
Name:        	GotoAddress
Parameters:  	x	Line number for display
			    y	Column number for display
				controller  [1,2, 3(both)]
Returns:        none
Description:    This function controls LCD writes to line 1 or 2 of the LCD.  
*****************************************************************************/
void GotoAddress(char mLine, char mColumn)
{
	BYTE controller=1;
	
	// Control at max values:
	if (mLine > 0x07)  mLine = 0x07;
	if (mColumn > 127)   mColumn = 127;
	x_address = mLine;
	y_address = mColumn;
	
	if (mColumn < 64) 
		controller=1; 
	else 
		controller=2;	
		
	if (mColumn > 63)    mColumn -= 64;
	
	LCD_write(CTRL, SET_X_ADDRESS+mLine, controller);
	WaitForNotBusy(controller);
	LCD_write(CTRL, SET_Y_ADDRESS+mColumn, controller);	
	WaitForNotBusy(controller);
}


/*****************************************************************************
Name:        	Busy
Parameters:  	controller 1 or 2 (not both)			
Returns:        1 if busy or reset in proc.
Description:    This function returns the busy or reset status
*****************************************************************************/
char Busy(char controller)
{
	char temp = 0;
	
	if (controller == BOTH)  return -1;
	
	temp = LCD_read(CTRL, controller);
	if (temp & 0x80)		// was 0x90
		return TRUE;
	else
		return FALSE;
}

/*****************************************************************************
Name:        	WaitForNotBusy
Parameters:  	controller must be 1 or 2
Returns:        none
Description:    This function waits for a controller to clear its busy bit
*****************************************************************************/
void WaitForNotBusy(char controller)
{
	while (Busy(controller)) 
	{
		  DisplayDelay(10); 
	};
}


/*****************************************************************************
Name:        	TransferBuffer
Parameters:  	position	Line number of display
			    *Buff		Buffer containing data to be written to display. 
			    			Last character should be null.
				controller  [1,2, 3(both)]
Returns:        none
Description:    This function sends data in the buffer to LCD writes.
				Writing beyond a controller limit will result in smooth
				transition to the next controller.				
*****************************************************************************/
void TransferBuffer(_far char * buffer)
{
	BYTE controller=1;
	do
	{
		LCD_write(DATA, *buffer++, controller);
	}
	while(*buffer);
}

/*****************************************************************************
Name:        	TransferBufferFixedLength
Parameters:  	position	Line number of display
			    *Buff		Buffer containing data to be written to display. 
			    			Last character should be null.
				controller  [1,2, 3(both)]
Returns:        none
Description:    This function controls LCD writes to line 1 or 2 of the LCD.  
*****************************************************************************/
void TransferBufferFixedLength( _far const char * string, BYTE length )
{
	BYTE controller =1;
	int i;		
	for (i=0; i < length; i++)
	{
		LCD_write(DATA,*(string+i),controller);
	}
}

/*	MAPPING OF DATA PINS - SCHEMATIC BASED
	DB0	- P10_0
	DB1 - P10_1 
	DB2 - P10_2 
	DB3 - P10_3 
	DB4 - P10_4 
	DB5 - P10_5 
	DB6 - P 8_1 
	DB7 - P 7_3 
*/

/*****************************************************************************
Name:           Remap_n_Send_Data
Parameters:     value - the value to write
Returns:        none
Description:    Takes the desired value and maps it into the actual hardware 
				pins.  
*****************************************************************************/
inline void LCD_Remap_n_Send_Data(BYTE value)
{
	/* Correct hardware wiring of data pins */
	DATA_PORT_0 = value;				// 	none in Rev B

}

/*****************************************************************************
Name:           LCD_Remap_Read_Data
Parameters:     value - the value to write
Description:    Reads the port values and maps according to the hardware
				values.
*****************************************************************************/
inline BYTE LCD_Remap_Read_Data()
{
	/* Correct hardware wiring of data pins */
	return DATA_PORT_0;
}

/*****************************************************************************
Name:           LCD_write
Parameters:     value - the value to write
				data_or_ctrl - To write value as DATA or CONTROL
								1 = DATA
								0 = CONTROL
Returns:        none
Description:    Writes data to display. Sends command to display.  
*****************************************************************************/
void LCD_write(unsigned char data_or_ctrl, unsigned char value, unsigned char controller)
{
	int shifted = 0;			// To correct for hardware wiring error,
								// data must be shifted right once.
	if ((data_or_ctrl == DATA) && (controller != BOTH))
	{
		// roll over to controller 2 if necessary:
		if (y_address > 63) controller = 2;
		if (y_address == 64) 
		{
		   LCD_write(CTRL, SET_X_ADDRESS+x_address, 2);
		   WaitForNotBusy(2);
		   LCD_write(CTRL, SET_Y_ADDRESS+0, 2);
		   WaitForNotBusy(2);
		}
	}

	// disable Interrupts.  We don't want data bus being 
	// 		a) written.  b) DDR changed.  c) ctrl lines
	DISABLE_IRQ		

	LCD_Port_Drive();
	EN_PIN = LOW;          		// Prepare cycle
	RW_PIN = WRITE;				// Setup Read Write
    DI_PIN = data_or_ctrl;     	// Data/Instruction SELECT (HIGH=DATA, LOW=CTRL)
	
	if (controller == 1)	
	{
		CS1_PIN = SELECTED;			// Chip Select Lines
		CS2_PIN = UNSELECTED;		
	}
	else if (controller == 2)
	{
		CS1_PIN = UNSELECTED;		
		CS2_PIN = SELECTED;			// Chip Select Lines
	}
	else if (controller == 3)		
	{
		CS1_PIN = SELECTED;			// Both
		CS2_PIN = SELECTED;			// 
	}
	
	LCD_Remap_n_Send_Data(value);

	DisplayDelay(0);					// We only need a very little delay
    EN_PIN = HIGH;          			// EN enable chip (HIGH)
	DisplayDelay(0);					// We only need a very little delay
    EN_PIN = LOW;          				// Latch data by dropping EN

	// Re-enable Interrupts
	ENABLE_IRQ			
	
	
	if(data_or_ctrl == DATA)
	{
		y_address++;
		if (y_address > 127)
		{
			x_address++;
			if (x_address > 7) x_address=0;
			controller = 1;
			y_address = 0;
			LCD_SendAddresses();
		}
	}
}

/*****************************************************************************
Name:           LCD_write_barebones
Parameters:     value - the value to write
				data_or_ctrl - To write value as DATA or CONTROL
								1 = DATA
								0 = CONTROL
Returns:        none
Description:    Does not handle smart controller switching
*****************************************************************************/
void LCD_write_barebones(unsigned char data_or_ctrl, unsigned char value, unsigned char controller)
{
	int shifted = 0;			// To correct for hardware wiring error,
								// data must be shifted right once.
	DISABLE_IRQ			// 
	
	LCD_Port_Drive();
	EN_PIN = LOW;          		// Prepare cycle
	RW_PIN = WRITE;				// Setup Read Write
    DI_PIN = data_or_ctrl;     	// Data/Instruction SELECT (HIGH=DATA, LOW=CTRL)

	if (controller == 1)	
	{
		CS1_PIN = SELECTED;			// Chip Select 1 
		CS2_PIN = UNSELECTED;			
	}
	else if (controller == 2)
	{
		CS1_PIN = UNSELECTED;			 
		CS2_PIN = SELECTED;			// Chip Select 2
	}
	else if (controller == 3)		
	{
		CS1_PIN = SELECTED;			//	Both
		CS2_PIN = SELECTED;			// 
	}
	
	/* Correct hardware wiring of data pins */
	LCD_Remap_n_Send_Data(value);

	DisplayDelay(0);					// We only need a very little delay
    EN_PIN = HIGH;          			// EN enable chip (HIGH)
	DisplayDelay(0);					// We only need a very little delay
    EN_PIN = LOW;          				// Latch data by dropping EN

	ENABLE_IRQ			// 
}


/*****************************************************************************
Name:           LCD_read
Parameters:     value - the value to write
				data_or_ctrl - To write value as DATA or CONTROL
								1 = DATA
								0 = CONTROL
Returns:        none
Description:    Writes data to display. Sends command to display.  
*****************************************************************************/
unsigned char LCD_read(unsigned char data_or_ctrl, unsigned char controller)
{
	int value  = 0;				// return value
	int shifted = 0;			// To correct for hardware wiring error,
								// data must be shifted right once
	LCD_Port_Read();
		
	EN_PIN = LOW;          		// Prepare cycle
	RW_PIN = READ;
    DI_PIN = data_or_ctrl;     	// DI SELECT (HIGH=DATA, LOW=CTRL)

	if (controller == 1)	
	{
		CS1_PIN = SELECTED;			// controller
		CS2_PIN = UNSELECTED;			// controller
	}
	else if (controller == 2)
	{
		CS1_PIN = UNSELECTED;		// 
		CS2_PIN = SELECTED;			// controller
	}
	else if (controller == 3)		// Both!
	{
		CS1_PIN = SELECTED;			//
		CS2_PIN = SELECTED;			// 
	}

	DisplayDelay(0);				// We only need a very little delay
    EN_PIN = HIGH;          		// EN enable chip (HIGH)
	DisplayDelay(0);				// We only need a very little delay

	/* read Data */
	/* Correct hardware wiring of data pins */
	value = LCD_Remap_Read_Data();
    EN_PIN  = LOW;         			// Latch data by dropping EN
	
	return value;
}


/*****************************************************************************
Name:          DisplayDelay 
Parameters:    unit                   
Returns:       none 
Description:   Delay routine for LCD display.   May be used for setup and 
			   hold times.
*****************************************************************************/
void DisplayDelay(unsigned long int units){

	unsigned long int counter = units * 0x100;

	while(counter--){
		_asm ("NOP");
		_asm ("NOP");
		_asm ("NOP");
	}
}




/*	Use this to determine any wiring order problem
	LCD_write(CTRL, 0x01 ,BOTH);
	LCD_write(CTRL, 0x02 ,BOTH);
	LCD_write(CTRL, 0x04 ,BOTH);
	LCD_write(CTRL, 0x08 ,BOTH);
	LCD_write(CTRL, 0x10 ,BOTH);
	LCD_write(CTRL, 0x20 ,BOTH);
	LCD_write(CTRL, 0x40 ,BOTH);
	LCD_write(CTRL, 0x80 ,BOTH); */
