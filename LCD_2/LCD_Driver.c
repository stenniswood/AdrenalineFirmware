/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Graphic LCD 128x64 Low level Display routines.	 *
 *											  						 *
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
 *	  PIN ASSIGNMENTS:												 *
 *				See "Proc_io_Defs.h"								 *
 *																	 *
 *    AUTHOR: Stephen Tenniswood, Software Engineer 	 			 *
 *			  May 7, 2007						 				 	 *
 *********************************************************************/
#include "iom16m1.h"
#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "pgmspace.h"
#include "pin_definitions.h"
#include "LCD_Driver.h"
#include "../core/leds.h"


#define TRUE 1
#define FALSE 0

/**********************************************************************************
* HARDWARE INTERFACE DEFINITIONS
**********************************************************************************/
#define DATA_PORT_0			 PORTB		// Data bus port
#define DATA_PORT_0_IN		 PINB		// Data bus port
#define PORT_DDR_0			 DDRB		// Data bus direction register
#define PORT_DDR_0_DRIVE	 0xFF		// Can't read DDR reg, so have can't OR values
#define PORT_DDR_0_RECEIVE 	 0x00		// 

#define DRIVE 1
#define HIGH  1
#define LOW   0

#define RESET_PIN		(LCD_RESET)		// 
#define RESET_PIN_MASK	(1<<LCD_RESET)	// 
#define RESET_PIN_DDR	DDRC			// 
#define ASSERT 		0
#define UNASSERTED 	1

#define CS1_PIN			LCD_CS_1		// 
#define CS1_PIN_MASK	(1<<CS1_PIN)	// 
#define CS1_PIN_DDR		DDRC			// 

#define CS2_PIN			LCD_CS_2		// 
#define CS2_PIN_MASK	(1<<CS2_PIN)	// 
#define CS2_PIN_DDR		DDRC			// 
#define SELECTED		1
#define UNSELECTED		0

#define RW_PIN 			LCD_RW  		// RS Register Select pin
#define RW_PIN_MASK		(1<<LCD_RW)		// bit mask from entire port
#define RW_PIN_DDR		DDRC			// RS pin DDR
#define READ 		1
#define WRITE 		0

#define DI_PIN			LCD_DI			// Data or Instruction select
#define DI_PIN_MASK		(1<<LCD_DI)		// bit mask from entire port
#define DI_PIN_DDR		DDRC			// EN pin DDR
#define DATA 		1
#define INSTRUCTION	0

#define EN_PIN			(LCD_ENABLE)  	// Display Enable pin
#define EN_PIN_MASK		(1<<LCD_ENABLE)	// bit mask from entire port
#define EN_PIN_DDR		DDRD			// EN pin DDR
#define ENABLED  	1					//
#define DISABLED 	0

#define BACKLIGHT_PIN		LCD_BACKLIGHT
#define BACKLIGHT_PIN_DDR	DDRC
#define BACKLIGHT_PIN_MASK	(1<<LCD_BACKLIGHT)

#define set_control_pin_ddr( mask, value )  { if (value) LCD_CONTROL_DDR |= mask; else LCD_CONTROL_DDR &= ~(mask); 	  }
#define set_control_pin( pin, value ) 		{ if (value) LCD_CONTROL_PORT|= (1<<pin); else LCD_CONTROL_PORT &= ~(1<<pin);   }
//#define set_control_pin_mask( mask, value ) { if (value) LCD_CONTROL_PORT|= (1<<pin); else LCD_CONTROL_PORT &= ~(1<<pin);   }

#define set_enable_pin_ddr( value ) { if (value) LCD_ENABLE_DDR |= EN_PIN_MASK; else LCD_ENABLE_DDR  &= ~(EN_PIN_MASK); }
#define set_enable_pin( value ) 	{ if (value) LCD_ENABLE_PORT|= EN_PIN_MASK; else LCD_ENABLE_PORT &= ~(EN_PIN_MASK); }

/**********************************************************************************
 * END HARDWARE INTERFACE DEFINITIONS
 **********************************************************************************/
// VARIABLES:
byte x_address = 0;
byte y_address = 0;
byte Active_Controller = 1;
byte PrevActive_Controller = 1;

/*************    ACCESSOR FUNCTIONS    ****************************/
/*****************************************************************************
Name:          	SendAddresses()
Parameters:     First call Set_y_address() or Set_x_address()
Description:    Send the internal x,y position counters to the display
				Ensures code is in-sync with the driver.
*****************************************************************************/
void SendAddresses()   {	GotoAddress(x_address,y_address);	}
/*****************************************************************************
Name:          	Port_Drive
Parameters:     none
Returns:        none
Description:    Initial data port direction to outputs (DDRB).
*****************************************************************************/
void Port_Drive()	{	PORT_DDR_0  |= PORT_DDR_0_DRIVE;	}	
/*****************************************************************************
Name:          	Port_Read
Parameters:     none
Returns:        none
Description:    Initial data port direction to inputs (DDRB).
*****************************************************************************/
void Port_Read()		{	PORT_DDR_0  &= PORT_DDR_0_RECEIVE;	}	

/*****************************************************************************
Name:           Send_Data
Parameters:     value - the value to write
Returns:        none
Description:    Takes the desired value and maps it into the actual hardware 
				pins.  The data is split between 3 ports.
*****************************************************************************/
inline void Send_Data(byte value)	{	DATA_PORT_0 = value;	}

/*****************************************************************************
Name:           Read_Data
Parameters:     value - the value to write
Description:    Reads the port values and maps according to the hardware
				values.
*****************************************************************************/
inline byte Read_Data()	{	return DATA_PORT_0_IN;	}

/******************************************
 *  Y is the length	is [0..127]
 *  X is the height is [0..8]
 ******************************************/
/****************************************************************
*  Following accessor functions used as external interface only; 
*  functions internal to this file handle on their own - smartly  
*****************************************************************/
byte Get_x_address() { return x_address; };
byte Get_y_address() { return y_address; };
byte Set_x_address(byte new_x) 
{
	x_address = new_x;
	if (x_address > 7) x_address = 7;
	return x_address;
};
byte Set_y_address(byte new_y)
{
	y_address = new_y;
	if (y_address > 127) y_address=127;
	if (y_address > 63) {
		//Active_Controller = 2;
		return (new_y - 64);		// address relative to chip controller
	}
	//else 
		//Active_Controller = 1;
	return y_address;
};
byte Get_controller_from_y()
{
	if (y_address < 64) 
		return 1;
	else 
		return 2;
}
byte update_controller_from_y() 
{ 
	Active_Controller = Get_controller_from_y(); 
	return Active_Controller;
}

/*****************************************************************************
Name:        	Increment_y_address()
Parameters:  	x	Line number for display 	[0..7]
			    y	Column number for display	[0..127]
Returns:        none
Description:    This function controls LCD writes to line 1 or 2 of the LCD.  
*****************************************************************************/
byte Increment_y_address()			// next column
{
	y_address++;
	if (y_address > 127)
	{
		Active_Controller = 1;
		Increment_x_address();	
		GotoAddress( x_address, 0 );	// this updates Active_Controller
	}		
	update_controller_from_y();
	
	if (PrevActive_Controller != Active_Controller) {
		PrevActive_Controller = Active_Controller;
		SendAddresses();
	}
	return y_address;
}
byte Increment_x_address()			// next line
{
	x_address++;
	return x_address;
}
/*****************************************************************************/
/*************    HARDWARE  RELATED  FUNCTIONS    ****************************/
/*****************************************************************************/
/*****************************************************************************
Name:        	send_controller_x_address()
Parameters:  	x	Line number for display 	[0..7]
Returns:        none
Description:    Sends the software x_address out to the controller.
				Ensures that we are in sync.
*****************************************************************************/
void send_controller_x_address()
{
	LCD_write		(CTRL, SET_X_ADDRESS+x_address, 0 );
	WaitForNotBusy	(0);
	LCD_write		(CTRL, SET_X_ADDRESS+x_address, 1 );
	WaitForNotBusy	(1);
}
/*****************************************************************************
Name:        	send_controller_y_address()
Parameters:  	x	Line number for display 	[0..7]
			    y	Column number for display	[0..127]
Returns:        none
Description:    Sends the software y_address out to the controller.
				Ensures that we are in sync.
*****************************************************************************/
void send_controller_y_address( )
{
	if (y_address<64)
	{
		LCD_write		(CTRL, SET_Y_ADDRESS+y_address, 0 );
		WaitForNotBusy	(0);
	} else {
		LCD_write		(CTRL, SET_Y_ADDRESS+(y_address-64), 1 );
		WaitForNotBusy	(1);
	}
}
/*****************************************************************************
Name:        	send_controller_xy_address()
Parameters:  	x	Line number for display 	[0..7]
			    y	Column number for display	[0..127]
Returns:        none
Description:    Sends the software y_address out to the controller.
				Ensures that we are in sync.
*****************************************************************************/
void send_controller_xy_address( byte controller, char x, char y)
{
	LCD_write		(CTRL, SET_X_ADDRESS+x, controller );
	WaitForNotBusy	(Active_Controller				   );
	LCD_write		(CTRL, SET_Y_ADDRESS+y, controller );
	WaitForNotBusy	(Active_Controller				   );
}
/*****************************************************************************
Name:        	GotoAddress
Parameters:  	x	Line number for display 	[0..7]
			    y	Column number for display	[0..127]
Returns:        none
Description:    This function controls LCD writes to line 1 or 2 of the LCD.  
*****************************************************************************/
void GotoAddress(char x, char y)
{	
	// Control at max values:
	x = Set_x_address( x );
	y = Set_y_address( y );
	update_controller_from_y();

	LCD_write		(CTRL, SET_X_ADDRESS+x, Active_Controller );
	WaitForNotBusy	(Active_Controller						  );
	LCD_write		(CTRL, SET_Y_ADDRESS+y, Active_Controller );
	WaitForNotBusy	(Active_Controller						  );
}


/*****************************************************************************
Name:          	InitDisplay
Parameters:     none
Returns:        none
Description:    Intializes the LCD display. 
*****************************************************************************/
void InitDisplay( void )
{
	// Pin Directions:
	Port_Drive();
	set_control_pin_ddr( BACKLIGHT_PIN,	 DRIVE );
	set_control_pin_ddr( CS1_PIN_MASK,	 DRIVE );
	set_control_pin_ddr( CS2_PIN_MASK,   DRIVE );
	set_control_pin_ddr( RW_PIN_MASK,	 DRIVE );
	set_control_pin_ddr( DI_PIN_MASK,	 DRIVE );
	set_control_pin_ddr( RESET_PIN_MASK, DRIVE );	
	set_enable_pin_ddr ( DRIVE );

	// Pin initial Values:
	set_control_pin( BACKLIGHT_PIN,	LOW 		);
	set_control_pin( CS1_PIN, 		SELECTED 	);
	set_control_pin( CS2_PIN, 		SELECTED 	);
	set_control_pin( RW_PIN	, 		WRITE		);
	set_control_pin( DI_PIN , 		INSTRUCTION	);
	set_control_pin( RESET_PIN, 	UNASSERTED	);
	set_enable_pin ( LOW );

	// RESET LCD
	set_control_pin( RESET_PIN, ASSERT   );
	DisplayDelay(1);
	set_control_pin( RESET_PIN, UNASSERTED );

	WaitForReset(1);
	
	// INITIALIZE REGISTERS:
	GotoAddress( 0,0 );
	LCD_write(CTRL, SET_DISPLAY_START+0x00, BOTH );
	WaitForNotBusy(1);
	DisplayDelay(1);
	LCD_write(CTRL, DISPLAY_ON, BOTH );
	WaitForNotBusy(1);	
}

/*****************************************************************************
Name:           SetupChipSelects()
Parameters:     value - the value to write
				data_or_ctrl - To write value as DATA or CONTROL
								1 = DATA
								0 = CONTROL
Description:    Does not handle smart controller switching
*****************************************************************************/
void setup_chip_selects(char controller)
{
	if (controller == 1)			// Setup the Chip Select Lines:
	{
		set_control_pin( CS1_PIN, SELECTED  );			// Left
		set_control_pin( CS2_PIN, UNSELECTED);
	}
	else if (controller == 2)
	{
		set_control_pin( CS1_PIN, UNSELECTED);		 
		set_control_pin( CS2_PIN, SELECTED  );			// Right
	}
	else if (controller == 3)		
	{
		set_control_pin( CS1_PIN, SELECTED );			// Both
		set_control_pin( CS2_PIN, SELECTED );
	}
}

/*****************************************************************************
Name:           LCD_write
Parameters:     value - the value to write
				data_or_ctrl - To write value as DATA or CONTROL
								1 = DATA
								0 = CONTROL
Description:    Does not handle smart controller switching
*****************************************************************************/
void LCD_write(unsigned char data_or_ctrl, unsigned char value,  char controller)
{
	Port_Drive();
	set_enable_pin (  LOW   );          			// Prepare cycle
	set_control_pin( RW_PIN, WRITE );				// Read/Write
    set_control_pin( DI_PIN, data_or_ctrl );     	// Data/Instruction SELECT (HIGH=DATA, LOW=CTRL)
	setup_chip_selects( controller );
	
	/* Correct hardware wiring of data pins */
	Send_Data(value);

	DisplayDelay(1);					// We only need a very little delay
    set_enable_pin( HIGH );          	// EN enable chip (HIGH)
	DisplayDelay(1);					// We only need a very little delay
    set_enable_pin( LOW );          	// Latch data by dropping EN
}

/*****************************************************************************
Name:           LCD_write_data
Parameters:     value - the value to write
				( implied DATA ) 
Description:    Writes data to display. Uses internal variables:
				x_address & y_address.
				Then selects the proper controller.
				Updates the addresses automatically.
*****************************************************************************/
void LCD_write_data(unsigned char value)
{
	// Adjust controller number if necessary
	//if ( controller != BOTH )
	{
		// Select controller based on y_address:
		//controller = Get_controller_from_y();

		// If first time writing to the opposite controller,
		// we need to send it the x_address.  Only the original
		// controller got the address before.
/*		if (Active_Controller != PrevActive_Controller) {
			PrevActive_Controller = Active_Controller;	
			SendAddresses();		// update Active_Controller	
		} */
	}
	LCD_write(DATA, value, Active_Controller);

	Increment_y_address();
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
	int value  = 0;								// return value	
	Port_Read();					
		
	set_enable_pin ( LOW  				  );    // Prepare cycle
	set_control_pin( RW_PIN, READ 		  );	// Read/Write
    set_control_pin( DI_PIN, data_or_ctrl );    // Data/Instruction (HIGH=DATA, LOW=CTRL)
	setup_chip_selects( controller 		  );

	DisplayDelay(1);				// Setup time
    set_enable_pin( HIGH );         // EN enable chip (HIGH)
	DisplayDelay(1);				// Hold time

	/* Read Data */
    set_enable_pin( LOW );  		// Latch data by dropping EN
	value = Read_Data();	
	return value;
}

/*****************************************************************************
Name:        	Busy - looks at DB7 (busy bit)
Parameters:  	controller 1 or 2 (not both)			
Returns:        1 if busy or reset in proc.
Description:    This function returns the busy or reset status
*****************************************************************************/
byte ReadBusyBit()
{
	set_enable_pin( LOW );  		// Latch data by dropping EN
	DisplayDelay(1);				// Setup time
    set_enable_pin( HIGH );         // EN enable chip (HIGH)
	DisplayDelay(1);				// Hold time
    set_enable_pin( LOW );  		// Latch data by dropping EN
	return Read_Data();
}
char Busy(char controller)
{
	char temp = 0;	
	if (controller == BOTH)  return -1;
	temp = ReadBusyBit();		
	if (temp & 0x80)			
		return TRUE;
	else
		return FALSE;
}
char InReset(char controller)
{
	char temp;
	if (controller == BOTH)  return -1;
	temp = ReadBusyBit();		
	if (temp & 0x10)		
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
	Port_Read();
	setup_chip_selects( controller );
	set_control_pin( RW_PIN, READ);
	set_control_pin( DI_PIN, INSTRUCTION);		
	while (Busy(controller))
	{
		  DisplayDelay(10); 
	}
}

/*****************************************************************************
Name:        	WaitForReset
Parameters:  	controller must be 1 or 2
Returns:        none
Description:    This function waits for a controller to finish resetting
*****************************************************************************/
void WaitForReset(char controller)
{
	Port_Read();
	setup_chip_selects( controller );
	set_control_pin( RW_PIN, READ);
	set_control_pin( DI_PIN, INSTRUCTION);		
	while (InReset(controller)) 
	{
		  DisplayDelay(10); 
	}
}

/*****************************************************************************
Name:          DisplayDelay 
Parameters:    unit                   
Returns:       none 
Description:   Delay routine for LCD display.   May be used for setup and 
			   hold times.
*****************************************************************************/
void DisplayDelay(unsigned long int units)
{
	volatile unsigned long int counter = units * 0x10;
	while(counter--){	}
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
	do {		
		LCD_write_data( pgm_read_byte(buffer));	
		buffer++;
	} while(*buffer);
}

void TransferBuffer_RAM(_far char * buffer)
{
	do
	{
		LCD_write_data(*buffer++);
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
				The string buffer must be located in PROGMEM space!
				
*****************************************************************************/
void TransferBufferFixedLength( _far const char * mData, byte length )
{
	int i;
	for (i=0; i < length; i++)
	{
		// read from PROGMEM and send:
		LCD_write_data( pgm_read_byte(mData+i));
	}
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
void TransferBufferFixedLength_RAM( _far const char * string, byte length )
{
	int i;
	for (i=0; i < length; i++)
	{
		LCD_write_data( *(string+i) );
	}
}

