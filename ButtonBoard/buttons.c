/*********************************************************************
Product of Beyond Kinetics, Inc

This code handles debouncing of the 16 buttons.

Strategy keep scaning until there is a difference.  Wait the debounce time,
re-read buttons.  If the difference is the same (buttons still pushed), then
consider good.

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "buttons.h"



/* We Want memory of each key.  This way we'll look for changes on any button, not
just row column.  Previous method of just looking at the column changes ignored when
two buttons pushed.  Ie. Push 1 button in a column (registers), while holding it down,
push another in same column.  This will be undetectable.  Also releasing the button
with the 2nd pushed would not have been detected.  SO... Keep memory of each key!
	Below are Samples of the key states.
*/
byte prev_keys[NUM_ROWS];
byte keys [NUM_ROWS];
byte keys2[NUM_ROWS];

// Array of keys pressed (sent in button pressed CAN message) :
byte NumberKeysPressed = 0;
byte key_index = 0;
char keys_down[NUM_ROWS];


/******************** ROW DDR (INPUT/OUTPUT) ******************/
void set_rows_output()
{
  ROW_DDR |= ROW_DDR_MASK;
}
void set_rows_input()
{
  ROW_DDR &= (~ROW_DDR_MASK);
}
void all_rows_low()
{  
  ROW_PORT &= (~ROW_DDR_MASK);
}
void all_rows_high()
{
  ROW_PORT |= ROW_DDR_MASK;
}

/********************************************************
 Then to isolate the row, put all rows high except for 1 
 *******************************************************/
void set_row(byte mRow, byte mHigh)
{
  switch (mRow)
  {
    case 0 : if (mHigh) ROW_PORT|= ROW0; else ROW_PORT &= ~ROW0; break;		// Front Row
    case 1 : if (mHigh) ROW_PORT|= ROW1; else ROW_PORT &= ~ROW1; break;    	// Top   Row
    case 2 : if (mHigh) ROW_PORT|= ROW2; else ROW_PORT &= ~ROW2; break;		// Row	2
    case 3 : if (mHigh) ROW_PORT|= ROW3; else ROW_PORT &= ~ROW3; break; 	// Row	3
    case 4 : if (mHigh) ROW_PORT|= ROW4; else ROW_PORT &= ~ROW4; break;		// Row  4
    default: break;
  }  
}

/******************** SWITCHES ******************/
void set_switches_input()
{	// internal pullup needed?  no because external
	SWITCH_DDR &= (~SWITCH_DDR_MASK);
}
void set_switches_output()
{
	SWITCH_DDR |= SWITCH_DDR_MASK;
}

/********************************************************
  return : result in lowest 4 bits 
	Buttons all pulled high.  So a press
	will always be a 0.
	0x6F => a button in both col 7 and 4 is pushed
	0xDF => 1101 a button in both col 5 is pushed
********************************************************/
byte read_switches()
{
  byte combined;
  combined = (SWITCH_IN_PORT & SWITCH_DDR_MASK);
  return combined;
}

 //static union uKeyRoster retval;
union uKeyRoster pack_array( byte* mKeyArray )
{
	static union uKeyRoster retval;
	byte index = 0;

	// combine odd rows into lower nibble.
	// all keys should only take 2.5 bytes
	// extra .5 byte stores count of keys pressed.
	for (int i=0; i<NUM_ROWS; i++)
	{
		if ((i%2)==0)
			retval.array[index] = (mKeyArray[i] & 0xF0);							// upper nibble
		else {
			retval.array[index] |= ((mKeyArray[i] & 0xF0) >> 4);		// lower nibble
			index++;
		}
	}
	return retval;
}

/***********************************************
Isolate the button's row:
Assuming Button is still pushed.  
(This has to be done quickly after a detect)
Return : stored in keys[] file scope variable
array[0]   Front Row
array[1]   Front Row (5)
array[2] 	 	 Row (4)
array[3] 	  	 Row (3)
array[4]   		 Row (2)
BITS are in UPPER NIBBLE!
************************************************/
void scan_keys( byte* mkeys )
{
  all_rows_high();
  for (int i=0; i<NUM_ROWS; i++)
  {
     set_row( i, 0 );
	 delay(CHARGE_TIME);
	 mkeys[i] = read_switches();	// upper nibble
     set_row( i, 1 );				// restore so not to affect the next row
  }
}

/*
Look for a difference between 'prev_keys[]' and 'keys[]'
Return:	TRUE - Difference found
		FALSE - No Change
*/
BOOL check_for_change( byte* mSrc1, byte* mSrc2 )
{  
  for (int i=0; i<NUM_ROWS; i++)
  {
	 if (mSrc1[i] != mSrc2[i])
	    return 1;
  }
  return 0;
}

void update_prev_keys()
{  
  for (int i=0; i<NUM_ROWS; i++)
	 prev_keys[i] = keys[i];
}

void buttons_init()
{
	for (int i=0; i<NUM_ROWS; i++)
		prev_keys[i] = 0xFF;		// assume all up.
	set_rows_output();
	//all_rows_low();
	set_switches_input();
	scan_keys(keys);
	update_prev_keys();
}

/***********************************************************
Return :   Pickup results in the "keys" array.
The column(s) affected are the 0 bits.
ie.  0xEF  means a button on SW1 column is pushed.
ie.  0x7F  means a button on SW4 & SW3 columns are pushed.
***********************************************************/
void wait_for_press()
{
	byte change;
	set_rows_output();
	all_rows_low();				// will show a press
	set_switches_input();		// switches are pulled high unless button is pushed

	// If there are no glitches _slower_ than 5ms.  This will work.
	// repeat until first sign of change:
	do {
		do { 
			scan_keys(keys);
			change = check_for_change(prev_keys, keys); 
		} while (change==0);

		// slight delay ~5ms
		delay(5);		// adjust time for 5ms!

		/* Reread : 
			if still different from previous, 
				then real debounce event
			if return to previous,
				then ignore the event		*/
			scan_keys( keys2 );
			change = check_for_change( prev_keys, keys2 );
	} while	( change==0 );	// the change has to remain for the debounce time or else not valid.

	// Real event!
	update_prev_keys();
}

/************************************************************
Converts the upper nibble of the column into an actual index
*************************************************************/
byte get_column(byte mColByte)
{
	//mColByte = ~mColByte;		// pushed will be a 1.  all others zero
	for (int i=4; i<8; i++)
		if ((mColByte & (1<<i))==0)
			return	i-4;
    return -1;
}

/************************************************************
	add_keys()  Add any pressed key in this row to the list of pressed keys.
				need to call this for every row.
	Each button has a number between [0..15]
INPUT:	mKeys
		mNumberKeysPressed
************************************************************/
void add_keys( byte mrow, byte mStartKeyNumber )
{
	// Scan each bit:
	for (int i=4; i<8; i++)
		if (  (mrow & (1<<i)) == 0  )	// pressed,
		{								// add to list
			keys_down[NumberKeysPressed++] = mStartKeyNumber+i-4;	
		}
}

/************************************************************
	add_keys()  Add any pressed key to the list of pressed keys
	Each button has a number between [0..15]
INPUT:	mKeys
OUTPUT:	mNumberKeysPressed
		keys_down[] 
		Front Edge:  Left to Right [ 0, 1, 2, 3 ]
		Top Row   :  			   [ 4, 5, 6, 7 ]
		2nd Row   :                [ 8, 9, A, B ]
		3rd Row   :                [ C, D, E, F ]
		4th Row   :                [ 10, 11, 12, 13 ]
************************************************************/
void build_key_list( byte* mkeys )
{
	NumberKeysPressed = 0;
	add_keys( mkeys[0], 0 );
	add_keys( mkeys[1], 4 );
	add_keys( mkeys[2], 8 );
	add_keys( mkeys[3], 12);
	add_keys( mkeys[4], 16);
}

/************** MULTI BUTTON PRESS READING ******************************
It is possible to read multiple buttons.  Even when on the same column
(by reading the rows with SWn as low.

FOR INITIAL UNIT TESTING, HOWEVER, ASSUMPTION IS ONLY 1 BUTTON At A TIME
will be pressed.
*************************************************************************/
#define MAX_ROW 5
#define MAX_COL 4

char assigned_characters[MAX_COL*MAX_ROW] = {
  'a', 'b', 'c', 'd',
  'e', 'f', 'g', 'h',
  'i', 'j', 'k', 'l',
  'm', 'n', 'o', 'p',
  '1', '2', '3', '4'
};

/***********************************
mRow - zero indexed ROW_DDR
mCol - zero indexed COL
************************************/
char lookup_char( byte mRow, byte mCol )
{
	byte index = (mRow * MAX_COL) + mCol;
	return assigned_characters[index];
}

