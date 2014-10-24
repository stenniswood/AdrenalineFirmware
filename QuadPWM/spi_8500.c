#include <avr/sfr_defs.h>
#include "iom16m1.h"
#include "inttypes.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "spi.h"
#include "can.h"
#include "pwm.h"
#include "spi_8500.h"


tPWM PWM_array	   [NUMBER_PWMS];		// each is a word 
tCOR CorrectionBits[NUMBER_PWMS];  		// Only upper 6 bits are valid
byte packed_array  [NUM_BYTES_48PWMS];	// used to send!

/* 
 Unpacks the 16 bits to 12 bits
 INPUT :	16 bit word array. highest nibble is empty.
 RETURN: 	output is in the "packed_array" file variable.
 */
void pack_array ( tPWM* mWordArray )
{
	int pwm_index    = NUMBER_PWMS-1;
	int packed_index = NUM_BYTES_48PWMS-1;
	do
	{
		packed_array[packed_index  ] = (( mWordArray[pwm_index] & 0x0FF0)  >>4);
		packed_array[packed_index-1] = (((mWordArray[pwm_index] & 0x000F)  <<4) + 
									   ((mWordArray[pwm_index-1] & 0x0F00)>>8));
		packed_array[packed_index-2] =  (mWordArray[pwm_index-1] & 0x00FF);
		packed_index -= 3;
		pwm_index -= 2;		
	} while (packed_index >= 2);
}

void init_Correction()
{
	// sets up so that Correction factor is 1:1.  See 8500 datasheet (2/3)*(64 + 32)/64 = 1	
	for (int i=0; i<NUMBER_PWMS; i++)
		CorrectionBits[i] = (63)<<6;		// was 64
}
void init_array()
{
	// sets up for full speed
	for (int i=0; i<NUMBER_PWMS; i++)
		PWM_array[i] = (0x0200 + i);
}

void init_packed( )
{
	for (int i=0; i<NUMBER_PWMS; i++)
		packed_array[i] = ( 0x55 );
}
/* This assumes that the 12 bit data is tightly packed.  ie. no 
gaps between.  It converts the 12 bit to 8 bit pointer and sends the
data.   */
void send_frame_12( uint8_t* min_array, uint8_t* mout_array, uint8_t mcommand )
{
	// START FRAME:
	LOWER_LDIBLANK();	

	// Most Significant Channel First (ie channel 48)  MSB first
	if (mout_array==NULL)
	{ 
		for (int i=(NUM_BYTES_48PWMS-1); i>=0; i--)		// down to
			spi_transfer( packed_array[i] );
	} else {
		for (int i=NUM_BYTES_48PWMS-1; i>=0; i--)		// down to
			mout_array[i] = spi_transfer( packed_array[i] );
	}
	spi_transfer( mcommand );

	// TERMINATE FRAME
	RAISE_LDIBLANK();
	delay(1);				// 1 here gives 4.36 us
							// 2 gives      6.48us 
	LOWER_LDIBLANK();	
}

void init_8500()
{
	init_Correction();		
	init_array();

	// SETUP DATA DIRECTION OF PINS
	CONTROL_DDR = 0;
	CONTROL_DDR |= CONTROL_OUTPUTS;

	// LDIBLANK GOES LOW:
	LOWER_LDIBLANK();	
	RAISE_OPEN_LED();

	pwm_init( 1200000., FALSE, TRUE );		// Call before the output_enable is sent	
	set_dutyB( 0.50 );
	
	/* For the 8500 chip:
		Clk Polar=0;
		Clk Phase=0 (rising edge);		*/
	// Uses normal MISO (not alternate) FALSE.  Is the Master TRUE.
	spi_init( FALSE, TRUE, MSBFIRST, SPI_MODE0 );
	init_packed();

	// SEND CORRECTION FACTOR:
	send_correction_frame(      );
	send_update_frame	 ( TRUE );
	send_output_enable	 ( TRUE );
}

/* The correction frame is normally done only once after
power up.  A change while running will not update the outputs
until the next Update frame.  So Call send_update_frame() after
this. */
void send_correction_frame( )
{
	pack_array( CorrectionBits );
	send_frame_12( NULL, NULL, CORRECTION_FRAME );
}

void send_update_frame( BOOL mSync )	// Update all 48 PWMs (12 bits each)
{
	pack_array(PWM_array);
	if (mSync)
		send_frame_12( NULL, NULL, SYNC_UPDATE_FRAME  );
	else
		send_frame_12( NULL, NULL, ASYNC_UPDATE_FRAME );
}

void send_output_enable( BOOL mEnable )
{
	byte cmd = OUTPUT_DISABLE_FRAME;
	if (mEnable)
		cmd = OUTPUT_ENABLE_FRAME;
	// data is irrelevant for this COMMAND!
	send_frame_12( NULL, NULL, cmd );
}

void send_phase_toggle_frame( )
{
	// data is irrelevant for this COMMAND!
	send_frame_12( NULL, NULL, PHASE_SHIFT_TOGGLE_FRAME );
}

void send_correction_toggle_frame( )
{
	// data is irrelevant for this COMMAND!
	send_frame_12( NULL, NULL, CORRECTION_TOGGLE_FRAME );
}

/*	Array consists of (bits) :
	0x0000 nnnn   hhhh llll 		// element 0
	0x0000 nnnn   hhhh llll 		// element 1	
	0x0000 nnnn   hhhh llll 		// ...
	0x0000 nnnn   hhhh llll 		// element 48

	Want to pack as :
	0xnn 			// element 0
	0xhh 			// element 0
	0xll 			// element 0

	0xhhhh llll 	(pwm[48] & 0x00FF)		
	0xllll nnnn 	(pwm[46] & 0x000F)<<4	+ (PWM[45] & 0x0F00)>>8	
	0xnnnn hhhh 	(pwm[46] & 0x0FF0)>>4		

	0xhhhh llll 	(pwm[47] & 0x00FF)			
	0xllll nnnn 	(pwm[48] & 0x000F)<<4	+ (PWM[47] & 0x0F00)>>8	
	0xnnnn hhhh		(pwm[48] & 0x0FF0)>>4
*/
