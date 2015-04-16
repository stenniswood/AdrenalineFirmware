/*********************************************************************
This code handles communications with the Maxim Analog chips.  
	There are 4 chips per board.  16 Readings per chip for a total
	of 64 readings.
	
DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
Product of Beyond Kinetics, Inc
********************************************************************/
#include <avr/sfr_defs.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
#include "configuration.h"
#include "calibrations.h"
#include "leds.h"
#include "analog.h"
#include "spi.h"


uint8_t 	ActiveSignalMask[8];
uint16_t	Readings[64];				// Analog Readings 
uint16_t	ReadingsPrev[64];			// Analog Readings 

void an_init()
{
	// Analog board uses regular MISO pins.  	TRUE
	// And is the master.  						TRUE

	// Maxim 11621 : 	
	// CS idles high
	// SCLK idles low.  10Mhz or less.
	// CPOL==CPHA  (must equal)
	spi_init( FALSE, TRUE, MSBFIRST, SPI_MODE0 );

	// 1 means output;  0 means input;	
	CS_DDR |= CS_DDR_MASK;		// Outputs!
	CS_DDR |= 0x80;				// SCLK Output!
	CS_DDR &= 0xFE;
	PORTB |= 0x01;

	EOC_DDR &= ~EOC_DDR_MASK;	// Inputs!
	an_reset_all();
	an_configure_all();
	
	//enable_pin_change_ints();
}

void enable_pin_change_ints()
{
	// ENABLE PIN CHANGE INTERRUPT:
	PCICR |= 0x02;		// pcint[8..15]
}
void disable_pin_change_ints()
{
	// DISABLE PIN CHANGE INTERRUPT:
	PCICR &= ~(0x02);	// pcint[8..15]
}

void select_chip(byte mChip)		
{
	switch (mChip)
	{
	case 0 : CS_PORT &= ~(1<<CS_A1);			 break;
	case 1 : CS_PORT &= ~(1<<CS_A2);			 break;
	case 2 : CS_PORT &= ~(1<<CS_A3);			 break;
	case 3 : CS_PORT &= ~(1<<CS_A4);			 break;
	default : break;			
	}
}

void deselect_chip( byte mChip )
{
	switch (mChip)
	{
	case 0 : CS_PORT |= (1<<CS_A1);			 break;
	case 1 : CS_PORT |= (1<<CS_A2);			 break;
	case 2 : CS_PORT |= (1<<CS_A3);			 break;
	case 3 : CS_PORT |= (1<<CS_A4);			 break;
	default : break;
	}	
}

void an_reset_all()  
{
	for (int i=0; i<4; i++)
		an_reset(i);
}

void an_configure_all()  
{
	for (int i=0; i<4; i++)
	{
		an_conversion   ( i, 0 );
		an_setup        ( i );
		an_averaging_off( i );
	}
}

void an_reset(byte mChip)		
{
	select_chip(mChip);
	spi_transfer( 0x10 );	// hard reset.	
	deselect_chip(mChip);
}

void get_chip_readings( byte mChip )
{
	// READ RESULT:
	for (int c=0; c<16; c++)
	{
		select_chip(mChip);
		byte upper = spi_transfer( 0x00 );
		deselect_chip(mChip);
		
		select_chip(mChip);
		byte lower = spi_transfer( 0x00 );
		deselect_chip(mChip);
		
		word result = (((word)upper)<<8) + (lower);
		result = (result>>2);

		// STORE RESULT : 
		ReadingsPrev[mChip*16+c] = Readings[mChip*16+c];
		Readings    [mChip*16+c] = result;
	}
}

/*
	mChannel - [0 to 15]
*/
void an_conversion(byte mChip, byte mChannel)
{
	select_chip(mChip);
	byte channel   = (mChannel<<3);
	byte scan_mode = SCAN_0_to_N;
	spi_transfer( 0x80|channel|scan_mode );
	deselect_chip(mChip);
	
	// WAIT for result:
	byte eoc_bit;
	switch (mChip)
	{	case 0 : eoc_bit = 0x10; break;
		case 1 : eoc_bit = 0x20; break;
		case 2 : eoc_bit = 0x40; break;
		case 3 : eoc_bit = 0x80; break;
		default: eoc_bit = 0x10; break;
	}
	while (EOC_PORT & eoc_bit) {   };

	// READ RESULTS:
	get_chip_readings( mChip );

}

void an_setup( byte mChip )
{
	select_chip(mChip);
	byte clock_mode = CLK_INT_AIN;
	byte ref_mode 	= REF_ALWAYS_ON;
	spi_transfer( 0x40| clock_mode | ref_mode );
	deselect_chip(mChip);	
} 

void an_averaging_off( byte mChip )
{
	select_chip(mChip);
	spi_transfer( 0x20 );
	deselect_chip(mChip);	
}

void an_averaging_on( byte mChip, byte mAverageMask )
{
	select_chip(mChip);
	// Not implemented the number results for scan mode 10 - single channel averaging.
	spi_transfer( 0x20| AVG_ON | mAverageMask );
	deselect_chip(mChip);	
}

word an_read( byte mChip )		
{
	select_chip(mChip);
	byte upper = spi_transfer( 0x00 );
	byte lower = spi_transfer( 0x00 );
	deselect_chip(mChip);	
	word result = (upper<<8);
	result |= lower;
	return (result>>2);		// 10 bits
}

// Read from the Maxim 11624 chips
void an_start_read(byte mChip)
{
	// Results come back in 16 bits:
		// 4 leading zeros:  0000
		// 10 bit data:          dddd dddd dd
		// 2 trailing zeros:			     00

	// Start a conversion by writing byte to the conversion register.
	static byte channel = 15;		// loops 0 thru 7 (same on all 4 chips!)
	an_conversion(mChip, channel);
}

// Read from the Maxim 11624 chips
void an_start_all()
{
	for (int i=0; i<4; i++)
	{
		byte configd = isConfigured( 1<<i );
		if (configd)
			an_start_read(i);
	}
}

void get_readings(byte mChip)
{
	for (int r=0; r<16; r++)
		Readings[mChip*16+r] = an_read(0);
}

#define CHIP_ONE   0x10
#define CHIP_TWO   0x20
#define CHIP_THREE 0x40
#define CHIP_FOUR  0x80

ISR ( PCINT1_vect ) /* PCINT[8..15] */
{ 
	int chip=-1;
	if ((EOC_IN & CHIP_ONE) == 0)
	{   
		chip = 0;
		get_chip_readings( chip );
	}
	if ((EOC_IN & CHIP_TWO) == 0)
	{   
		chip = 1;
		get_chip_readings( chip );		
	}
	if ((EOC_IN & CHIP_THREE) == 0)
	{   
		chip = 2;
		get_chip_readings( chip );		
	}
	if ((EOC_IN & CHIP_FOUR) == 0)
	{   
		chip = 3;
		get_chip_readings( chip );
	}
}


static short ReportCounter = 1;		// when zero sends update
void analog_timeslice()
{
	// Time To Report:
	if (isReportingEnabled())
	{
		ReportCounter--;
		if (ReportCounter<=0)
		{
			//led_on(4);		
			ReportCounter = getReportRate();	// Refill 

			// Initiate a conversion for each active chip  (isr will handle data)
			an_start_all();
			// Will this buffer up the messages or block until all are sent?!
			// probably need to redesign
			//led_off(4);
		}
	}
}


