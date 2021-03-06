/*******************************************************************
 ATMEL SIDE OF THINGS.  RECEIVES SPI DATA FROM RPI!

Use a binary protocol.  Simple exchange.  1 byte will indicate
the command.

Product of Beyond Kinetics, Inc
This core code handles SPI communications hardware

Notes on Initial Debug with PWM generator board:
		Several problems int the "core" spi.c file.
	A)	The ddr pins were oring with MOSI and not (1<<MOSI).
		This cause the SCLK (1<<SCLK) to not operate
	B)  Have to force SS pin to be an output to prevent it 
		from going into slave mode.  Believe this is why
		the SPIF flag was not getting set; module went into 
		slave mode and was expecting an external clock!
	C)  The alternate pins also was missing the 1<<  (1<<SPIPS)

	After this, the motor ran! 

DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdlib.h>
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "inttypes.h"
#include "interrupt.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "spi.h"
#include "leds.h"
#include "servos.h"
#include "rpi_spi.h"
#include "lowside.h"
#include "analog.h"

float pan;
float tilt;
byte  LS_Value;
byte  LS_Values;
byte  LS_Bit;
float Analog_Values[8];
byte  Channel;

byte  InBuffer [20];
byte  OutBuffer[20];
byte  InIndex  = 0;
byte  OutIndex = 0;
byte  num_expected=0;
	

// Use fixed point
void parse_pan_tilt_angles(byte* mData, float *pan, float *tilt)
{
	// Atmel is slave, so we have received data.
	short lpan,ltilt;
	lpan  = mData[1];   lpan=(lpan<<8);	
	lpan  |= mData[2]; 
	
	ltilt = mData[3];	ltilt=(ltilt<<8);
	ltilt |= mData[4];

	*pan  = ((float)lpan)/100.0;
	*tilt = ((float)ltilt)/100.0;
}

void parse_low_side_driver(byte* mData, byte* mBit, byte* mValue)
{
	// Atmel is slave, so we have received data:
	*mBit   = mData[1];
	*mValue = mData[2];
}

void parse_low_side_drivers(byte* mData, byte* mValues)
{
	// Atmel is slave, so we have to request a read.
	*mValues = mData[1];
}

void parse_read_analog( byte* mData, byte* mChannel )
{
	// Atmel is slave, so we have to request a read.
	*mChannel = mData[1];
}

void parse_read_all_analogs( byte* mData, float* mValue )
{
	// No parsing to be done.  See packaging instructions.
}

/**************************************************************/
// from Analog.h :
void pack_all_analogs( )
{
	byte offset = 1;
	for (int i=0; i<8; i++)
	{
		OutBuffer[i*2+offset]  = hi( Sample[i] );
		OutBuffer[i*2+offset]  = lo( Sample[i] );
	}
}
/**************************************************************/

byte num_expected_bytes( byte mToken )
{
	switch ( mToken )
	{
	case UPDATE_PAN_TILT 		: return 5;		break;
	case UPDATE_LOWSIDE_DRIVER 	: return 3;		break;
	case UPDATE_LOWSIDE_DRIVERS : return 2;		break;
	case READ_ANALOG 			: return 1;		break;
	case READ_ANALOGS 			: return 1;		break;	
	default: break;
	}
	return 0;	
}

#define TEST_SERVO 1
void test_rx_response( byte mData )
{
	switch ( mData )
	{
	case 1		: set_angle(TEST_SERVO, -50.0);	break;
	case 2		: set_angle(TEST_SERVO, -25.0);	break;
	case 3		: set_angle(TEST_SERVO,   0.0);	break;
	case 4		: set_angle(TEST_SERVO,  25.0);	break;
	case 5		: set_angle(TEST_SERVO,  50.0);	break;	
	case 0x80	: set_angle(TEST_SERVO,  50.0);	break;	
	case 0x40	: set_angle(TEST_SERVO, -25.0);	break;		
	case 0xC0	: set_angle(TEST_SERVO,   0.0);	break;			
	case 0x20	: set_angle(TEST_SERVO, +25.0);	break;				
	default 	: set_angle(TEST_SERVO,   0.0);  break;
	}
}
void zero_output_buffer()
{
	for (int i=0; i<20; i++)
		OutBuffer[i] = 0xFF;
}

void test_servo()
{
	while(1)
	{
		set_angle(1, -30.0);
		set_angle(2, -30.0);		
		delay(10000);
		set_angle(1, +30.0);
		set_angle(2, +30.0);
		delay(10000);
	}
}
// Called everytime a byte has been received.
byte spi_callback()
{							set_angle(1, -25.0);

	InBuffer[InIndex] = SPDR;
/*	if (InIndex==0)
	{
		num_expected = num_expected_bytes( InBuffer[0] );
		zero_output_buffer();
		OutBuffer[1] = 0x99;			// Acknowledge start of new command.
		OutBuffer[2] = InBuffer[0];		// echo command
		OutBuffer[3] = num_expected;	// bytes expected.
	} */
	InIndex++; 
	SPDR = OutIndex++;	//OutBuffer[]

/*	if (InIndex == 5) 		// was num_expected
	{
		test_rx_response( InBuffer[0] );
		//distribute_parsing( InBuffer );
		InIndex = 0;
		OutIndex = 0;
	}*/	
	return 1;
}

void distribute_parsing( byte* mData )
{
	switch ( mData[0] )
	{
	case UPDATE_PAN_TILT : 
				// After all data has been received.
				parse_pan_tilt_angles(mData, &pan, &tilt);
				set_angle(1, pan );
				set_angle(2, tilt);
				break;
	case UPDATE_LOWSIDE_DRIVER :
				// After all data has been received.
				parse_low_side_driver(mData, &LS_Bit, &LS_Value);
				if (LS_Value)
					turn_output_on (LS_Bit );
				else
					turn_output_off(LS_Bit );					
				break;
	case UPDATE_LOWSIDE_DRIVERS : 
				// After all data has been received.	
				parse_low_side_drivers(mData, &LS_Values );
				break;
	case READ_ANALOG : 
				// Needs to be called after the first byte arrives!!!
				parse_read_analog(mData, &Channel);
				// SUPPLY DATA NOW!				
				break;
	case READ_ANALOGS :
				// Measurements are being continuously taken.
				// Needs to be called after the first byte arrives!!!	
				// SUPPLY DATA NOW!
				pack_all_analogs( );								
				break;
	default:  break;	
	}
}

