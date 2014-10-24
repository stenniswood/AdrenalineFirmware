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
#include "leds.h"
#include "analog.h"
#include "spi.h"


uint8_t 	ActiveSignalMask[8];
uint16_t	Readings[64];			// Analog Readings 

void an_init()
{
	// Analog board uses alternate MISO_A pins.  TRUE
	// And is the master.  						 TRUE
	spi_init( TRUE, TRUE, MSBFIRST, SPI_MODE0 );
}

// Read from the Maxim 11624 chips
void an_read_actives()		
{
		
}

void an_apply_filters()
{

}




