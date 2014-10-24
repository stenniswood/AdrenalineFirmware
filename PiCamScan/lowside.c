/*********************************************************************
Product of Beyond Kinetics, Inc

This code handles Lowside drivers hardware.

	There are 7 outputs going into the ULN2003A darlington array.
	Each transistor rated at 500mA.  (So the high side should come directly from the power source, 
		and not the regulated voltage (5V).  This allows a higher voltage (more flexible circuits)
		and doesn't burden the 2A regulator.

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <avr/sfr_defs.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
#include "leds.h"
#include "buttons.h"
#include "can_id_list.h"
#include "can_instance.h"
#include "can_eid.h"
#include "can_board_msg.h"
#include "lowside.h"


void lowside_init()
{
	// set all to off
	for (int i=1; i<8; i++)
		turn_output_off( i );

	// set dio pins as outputs
	LOWSIDE_DDR_1 |= (LOWSIDE_1 | LOWSIDE_2);
	LOWSIDE_DDR_2 |= (LOWSIDE_3 | LOWSIDE_6 | LOWSIDE_7);
	LOWSIDE_DDR_3 |= (LOWSIDE_4 | LOWSIDE_5);

}

void set_lowside_byte(byte mValue)
{
	byte test_bit;
	for (int i=0; i<8; i++)
	{
		test_bit = (1<<i);
		if (mValue & test_bit)
			turn_output_on (i);
		else 
			turn_output_off(i);
	}
}

void turn_output_on( byte bit_number )
{
	switch (bit_number)
	{
	case 1:		LOWSIDE_PORT_1 |= LOWSIDE_1;
				break;
	case 2:		LOWSIDE_PORT_1 |= LOWSIDE_2;
				break;
	case 3:		LOWSIDE_PORT_2 |= LOWSIDE_3;
				break;
	case 4:		LOWSIDE_PORT_3 |= LOWSIDE_4;
				break;
	case 5:		LOWSIDE_PORT_3 |= LOWSIDE_5;
				break;
	case 6:		LOWSIDE_PORT_2 |= LOWSIDE_6;
				break;
	case 7:		LOWSIDE_PORT_2 |= LOWSIDE_7;
				break;
	default: break;
	}
}

void turn_output_off( byte bit_number )
{
	switch (bit_number)
	{
	case 1:		LOWSIDE_PORT_1 &= ~LOWSIDE_1;
				break;
	case 2:		LOWSIDE_PORT_1 &= ~LOWSIDE_2;
				break;
	case 3:		LOWSIDE_PORT_2 &= ~LOWSIDE_3;
				break;
	case 4:		LOWSIDE_PORT_3 &= ~LOWSIDE_4;
				break;
	case 5:		LOWSIDE_PORT_3 &= ~LOWSIDE_5;
				break;
	case 6:		LOWSIDE_PORT_2 &= ~LOWSIDE_6;
				break;
	case 7:		LOWSIDE_PORT_2 &= ~LOWSIDE_7;
				break;
	default: break;
	}
}
