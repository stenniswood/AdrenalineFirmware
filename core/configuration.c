/*********************************************************************
Product of Beyond Kinetics, Inc
Core Adrenaline Firmware configuration.
--------------------------------------------
DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <string.h>
#include "bk_system_defs.h"
#include "pgmspace.h"
#include "inttypes.h"
#include "pin_definitions.h"
#include "can_id_list.h"
#include "can.h"
#include "can_eid.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "can_msg.h"
#include "leds.h"
#include "configuration.h"
#include "eeprom.h"

/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/
byte config_byte_1 	= 0x00;			// the actual use of the bits herein shall be defined 
byte config_byte_2 	= 0x00;			// by the application through use of the isConfigured() 
byte config_byte_3 	= 0x00;			// macro.
byte config_byte_4 	= 0x00;
byte sys_config_byte= 0x00;			// Protected can only be modified by system! Not directly user modifiable bits! (contains NewBoard bit)

// This must be defined in the app - "calibrations.c" file.
extern void app_default_configuration();

/* return 1 = NEW BOARD OKAY
		  0 = bad password 
	Caller:		None found in BigMotorEn.
 */
byte make_system_new_board( word passcode )
{
	byte retval = 0;
	if (passcode == 0x1234)
	{
		sys_config_byte &= ~CAN_NEW_BOARD;		// 0 means new board!
		//save_configuration_eeprom( (byte*)INSTANCE_EEPROM_ADDRESS+2 );
		cli(); save_configuration(); sei();
		retval = 1;
	}
	return retval;
}

/**********************************************
can_parse_configure_request()
Return	:	The module configuration bytes
Also want to save:  Current thresholds.
Report DTCs (overtemperature, over voltage)
***********************************************/
void can_parse_configure_request( sCAN* mMsg )
{
	byte index  =  mMsg->data[0];	// which config_x byte
	byte mask   =  mMsg->data[1];	// mask (which bits are changing)
	byte value  =  mMsg->data[2];	// new values 
	switch (index)					// alternatively could make config_bytes[4] an array
	{
		case 1 : config_byte_1 |=  (mask & value);
				 config_byte_1 &= ~(mask & (~value));
				 break;
		case 2 : config_byte_2 |=  (mask & value);
				 config_byte_2 &= ~(mask & (~value));
				 break;
		case 3 : config_byte_3 |=  (mask & value);
				 config_byte_3 &= ~(mask & (~value));
				 break;
		case 4 : //config_byte_4 |=  (mask & value);
				 //config_byte_4 &= ~(mask & (~value));
				 //Don't want user to change right now.
				 return ;
				 break;
		default: break;
	}
	// Save is done in can_board_msg timeslice (avoiding in the isr() corrupt data)
	//cli();  save_configuration(); sei();
	//save_configuration_eeprom( (byte*)INSTANCE_EEPROM_ADDRESS+2 );  sei();
	//		// save everything anyway.
}

/* Set the config_byte_n bytes to default first time the board is run. CAN_NEW_BOARD */
void default_configuration()
{
	config_byte_1 = 0;
	config_byte_2 = 0;		// no reports
	config_byte_3 = 0;		// 
	config_byte_4 = (CAN_BAUD_250K | NORMAL_HISTORY_OP_MODE);	

	// Make sure CAN_NEW_BOARD goes to one!
	// BECAUSE read_configuration_eeprom() reads with a ~
	// the CAN_NEW_BOARD will be a 0 after reflash
	// ALSO SAVED with a ~, so to make as a not new board, 
	init_complete    = FALSE;
	sys_config_byte |= CAN_NEW_BOARD;		// means no longer new board
	Confirmed        = NOT_CLAIMED; 		// will force MyInstance to init
	MyInstance 		 = 0x98;

	//led_on(4);
}

void config_init()
{
	//read_configuration( ); // would like to remove and force each app to call prior to.
							// b/c BigMotorEn board already does and we should have a consistent interface.
							// and b/c it's better the user knows when it's read.
	//led_off(4);
	
	// BECAUSE read_configuration_eeprom() reads with a ~, 
	// The CAN_NEW_BOARD bit will be a 0 after reflash (~1)
	if (isSysConfigured(CAN_NEW_BOARD)==FALSE)		// Active Low
	{												// First time this board has run!
		default_configuration();					// For the core modules (see above)
		app_default_configuration();				// For application (board specific config) ie motor boards need stops

		// Don't save here! Low voltage during power up causes eeprom corruption.
		// Also it's not needed here.  If it's a new board, then defaults get set
		// as above and if it doesn't complete an instance, then it will continue
		// trying on boot until it does.  This is the expected.  And it keeps the new board
		// status until an instance is assigned (happens in can_instance_timeslice())
		// See: can_instance_timeslice()  if (Confirmed == DIRTY)...
		// save_configuration();
	}	
}

byte* save_configuration_di()
{
	cli();
	return save_configuration();
	sei();
}

/**********************************************
save_configuration()
Return	:	The module configuration bytes
Also want to save:  Current thresholds.
Report DTCs (overtemperature, over voltage)
***********************************************/
byte* save_configuration()
{
	byte* Ptr;
	Ptr = save_instance_number (INSTANCE_EEPROM_ADDRESS);
	Ptr = save_confirmed_status(Ptr);
	return save_configuration_eeprom( Ptr );
}
/**********************************************************
read_configuration(byte mStartAddress)
Reads from EEPROM, and stores into the RAM
sEndPoint structures defined in pot.h
***********************************************************/
byte* read_configuration()
{
	byte* Ptr;
	Ptr = read_instance_number (INSTANCE_EEPROM_ADDRESS);
	Ptr = read_confirmed_status(Ptr);
	return read_configuration_eeprom(Ptr);
}

/*************************************************
save_instance_number()
Return	:	The instance number within the network
**************************************************/
byte* save_instance_number( byte* mEEPROMAddress )
{
    eeprom_write_byte( (unsigned char*)mEEPROMAddress,   MyInstance );
    return mEEPROMAddress+1;
}
byte* read_instance_number(byte* mEEPROMAddress)
{
    MyInstance = eeprom_read_byte( (unsigned char*)mEEPROMAddress );
    return mEEPROMAddress+1;    
}

/**********************************************
save_confirmed_status()
Return	:	The 
***********************************************/
byte* save_confirmed_status(byte* mEEPROMAddress)
{
	//Confirmed = ~Confirmed;  	// flip all bits b/c after erase, 0xFF.
	// NO, don't do the flip b/c we have the config_init() routine now
	// which is done 1 time after reflash (uses 1 bit in config_byte_4)
    eeprom_write_byte( (unsigned char*)mEEPROMAddress, Confirmed  );
    return mEEPROMAddress+1;
}
byte* read_confirmed_status(byte* mEEPROMAddress)
{
    Confirmed  = eeprom_read_byte( (unsigned char*)mEEPROMAddress );
    return mEEPROMAddress+1;
}

extern bool okay_to_read_cal;

/**********************************************
save_configuration_eeprom()
Return	:	The module configuration bytes
Also want to save:  Current thresholds.
Report DTCs (overtemperature, over voltage)
***********************************************/
byte* save_configuration_eeprom(byte* addr)
{
	byte* address = addr;
	eeprom_write_byte( addr++, ~config_byte_1 );
	eeprom_write_byte( addr++, ~config_byte_2 );	
	eeprom_write_byte( addr++, ~config_byte_3 );	
	eeprom_write_byte( addr++, ~config_byte_4 );
	eeprom_write_byte( addr++, ~sys_config_byte);
	okay_to_read_cal = true;
	return addr;
}

/**********************************************************
read_configuration_eeprom(byte mStartAddress)
Reads from EEPROM, and stores into the RAM
sEndPoint structures defined in pot.h
***********************************************************/
byte* read_configuration_eeprom(byte* addr)
{
	config_byte_1 = ~(eeprom_read_byte( addr++ ));
	config_byte_2 = ~(eeprom_read_byte( addr++ ));
	config_byte_3 = ~(eeprom_read_byte( addr++ ));
	config_byte_4 = ~(eeprom_read_byte( addr++ ));	
	sys_config_byte = ~(eeprom_read_byte( addr++ ));
	return addr;
}
