#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#define BOOL byte

/*************************************************************
  ROWS ARE NATURALLY PULLED HIGH
  SWITCHES ARE NATURALLY PULLED HIGH

	To read the key pad, we could:
	A) Wait for change on the SW1..4 columns
		1) Pull all Rows low and 
		2) Sense the [SW1..SW4] (interrupt on change)
		
	B) Wait for change on the ROWs
		1) Pull all [SW1..4] low
		2) Sense the [ROW0..ROW4] (interrupt on change)
************************************************************/

/************** PIN DEFINITIONS *********************/
#define ROW_DDR_MASK 0x7C
#define ROW_DDR DDRB
#define ROW_PORT PORTB

// ROWS ARE NATURALLY PULLED HIGH
#define ROW0 (1<<2)
#define ROW1 (1<<3)
#define ROW2 (1<<4)
#define ROW3 (1<<5)
#define ROW4 (1<<6)

#define SWITCH_DDR_MASK 0xF0
#define SWITCH_DDR 		DDRC
#define SWITCH_PORT 	PORTC
#define SWITCH_IN_PORT 	PINC

// SWITCHES ARE NATURALLY PULLED HIGH
#define SW1 (1<<4)
#define SW2 (1<<5)
#define SW3 (1<<6)
#define SW4 (1<<7)

union uKeyRoster 
{
  long int full;
  byte  array[4];
};

// The stored keypad results:
extern byte prev_keys[];
extern byte keys[];
extern byte keys2[];


void set_rows_output		(			);
void set_rows_input			(			);
void all_rows_low			(			);
void all_rows_high			(			);
void set_switches_input		(			);
void set_switches_output	(			);
byte read_switches			(			);
void pull_1row_low			(byte mRow	);
void configure_buttons_wait	(			);

void wait_for_press			(			);
union uKeyRoster pack_array ( byte* mKeyArray );

byte get_column			    (byte mColByte);
void scan_keys				( byte* mkeys );

char lookup_char			( byte mRow, byte mCol );


#define one_second 100000	// approximately
#define CHARGE_TIME   160   // Time constant is 10n * 1k = 10us   Clock runs at 16Mhz.  There 160 gives 10us
void delay					( long int mdelay );



/* 
When pins are high impedance:
ROWx are normally high
SWx  are normally high

To wait for button press, put all rows low and SWx as inputs.
any 1 going to zero means pressed.
(Only 5ma * 5 = 25ma drain )

*/

void configure_buttons();
void read_buttons	  ();


#endif
