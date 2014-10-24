/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Low Level Routines RTOS					 	 	 *
 *											  						 *
 *	  CATEGORY	 :   BIOS Driver									 *
 *											   						 *
 *    PURPOSE	 : Save and Recall Application Specific 			 *
 *					settings from EEPROM.							 *
 *																	 *
 *	  HARDWARE RESOURCES:									   		 *
 *			 Atmega16m1 EEPROM								 		 *
 *																	 *
 *	  APPLICATION EEPROM DATA:										 *
 *			EndPoint1 (value & angle)								 *
 *			EndPoint2 (value & angle)								 *
 *			Motor Stall Current 1									 *
 *			Motor Stall Current 2									 *	
 *			Latest Position (EncoderCount)							 *
 *											   						 *
 *    AUTHOR: Stephen Tenniswood, Robotics Engineer 	 			 *
 *		  	  November 2013						 				 	 *
 *			  Product of Beyond Kinetics, Inc						 *
 *		  	  Revised for atmel Sept 2013						 	 * 
 *********************************************************************/
/**********************************************************
	EEPROM OVERVIEW - ADRENALINE FIRMWARE BIGMOTOR BOARD

TOP LEVEL : save_cal()   in file   calibrations.c

calibrations.c::save_cal();
calibrations.c::	save_stops_eeprom();
calibrations.c::	save_currentThresholds_eeprom();
calibrations.c::	save_latest_position_eeprom();
configuration.c::	save_configuration() stores the following 3 items:
configuration.c::		save_instance_number()		- Instance Number
configuration.c::		save_confirmed_status()		- 
configuration.c::		save_configuration_eeprom()	- 4 config_byte_n's

WHEN ARE ITEMS SAVED?
	On a minted board, the EEPROM will be all 1's (erase is 1; programmed is 0; verify this is same on eeprom as on flash)
	The confirmed status will therefore have to be 0 when true.  Active low.
	
A) After the instance init thing runs and gets a valid id, 
		Call: save_instance_number() and save_confirmed_status() only (not the save_configuration())
		No further calls needed. It will get called again when save_cal is called perhaps remove this
				from save_cal()
B) Upon receiving a _INSTANCE_ASSIGN message.
C) Upon receiving a _INSTANCE_RESTART message.

Bet anything the problem is with the confirmed status negated thing.
***********************************************************/
#include <stdlib.h>
#include <math.h>
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "motor.h"
#include "can_instance.h"
#include "pot.h"
#include "pwm.h"
#include "eeprom.h"
#include "adc.h"
#include "calibrations.h"
#include "configuration.h"
#include "encoder.h"

/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/
struct sEndPoint 	EndPoint1;
struct sEndPoint 	EndPoint2;
struct sEndPoint 	Range;			// Computed from EndPoint1 & 2

word current_1_threshold = 0x3FF;
word current_2_threshold = 0x3FF;

/***************************************************************
   This is called after system default configuration. 
	Only 1 time for the life of the board (or until it is reflashed)  
	To set the application defaults.		
 ***************************************************************/	
void app_default_configuration()
{
	config_byte_1 = 0;
	config_byte_2 = 0;
	compute_default_stops();
	
	// Default Thresholds:
	current_1_threshold = 0x7FF;
	current_2_threshold = 0x7FF;
}

/* Callback for configuration change. */
void config_change( byte mByteChanged )
{
	if (mByteChanged==1)
	{
		// Update BaseFrequency (even if not changed)
		SetBaseFrequency();
	}
}

/**********************************************************
save_cal()		Save all EEPROM stored variables & constants.

Saves to EEPROM 
***********************************************************/
byte* save_cal()
{
	byte* addr;
	addr = save_configuration			(    );		// MyInstance & Confirmed & config bytes
	addr = save_stops_eeprom			(addr);
	addr = save_currentThresholds_eeprom(addr);
	addr = save_latest_position_eeprom  (addr);
	return addr;
}

/************************************************************
read_cal()		Read all EEPROM stored variables & constants.

Read from EEPROM, and stores into the RAM
*************************************************************/
byte* read_cal()
{
	byte* addr;
	addr = read_configuration();		// myinstance, confirmed status, config bytes
	addr = read_stops_eeprom			(addr);
	addr = read_currentThresholds_eeprom(addr);
	addr = read_latest_position_eeprom	(addr);
	return addr;
}

/*******************************************************
save_stops_eeprom()		

Save both structures EndPoint1 & EndPoint2.
RETURN:		Last address written + 1 (ie. next available
			EEPROM address)
*******************************************************/
byte* save_stops_eeprom(byte* addr)
{
	byte size = sizeof(sEndPoint);
	// Save only EndPoint1 & EndPoint2:

	// END POINT 1:
	byte* ptr = (byte*)&EndPoint1;
	for (int i=0; i<size; i++)
		eeprom_write_byte( addr++, *(ptr++) );

	// END POINT 2:
	ptr = (byte*)&EndPoint2;
	for (int i=0; i<size; i++)
		eeprom_write_byte( addr++, *(ptr++) );	
	return addr;
}
/**********************************************************
read_stops_eeprom()

Reads both structures EndPoint1 & EndPoint2 from EEPROM, 
and stores into the RAM.
***********************************************************/
byte* read_stops_eeprom(byte* addr)
{
	// Save only EndPoint1 & EndPoint2
	byte size = sizeof(sEndPoint);

	byte* ptr = (byte*)&EndPoint1;
	for (int i=0; i<size; i++)
		*(ptr++) = eeprom_read_byte( addr++ );

	ptr = (byte*)&EndPoint2;
	for (int i=0; i<size; i++)
		*(ptr++) = eeprom_read_byte( addr++ );
	
	compute_range();
	return addr;
}

/***************************************************
save_currentThresholds_eeprom()
Return			 :	The module configuration bytes
Also want to save:  Current thresholds.
Report DTCs (overtemperature, over voltage)
****************************************************/
byte* save_currentThresholds_eeprom(byte* addr)
{
	// CURRENT THRESHOLD (for shutoff)
	// SAMPLING IS 10 BITS ONLY.
	eeprom_write_byte( addr++, lo(current_1_threshold>>2) );	
	eeprom_write_byte( addr++, lo(current_2_threshold>>2) );	
	return addr;
}
/**********************************************************
read_currentThresholds_eeprom(byte mStartAddress)

Reads from stall currents from EEPROM, and stores into RAM
***********************************************************/
byte* read_currentThresholds_eeprom(byte* addr)
{	// CURRENT THRESHOLD (for shutoff)	
	current_1_threshold = ((eeprom_read_byte( addr++ ))<<2);
	current_2_threshold = ((eeprom_read_byte( addr++ ))<<2);
	return addr;
}

/***************************************************
save_latest_position_eeprom()
Return			 :	The module configuration bytes
Also want to save:  Current thresholds.
Report DTCs (overtemperature, over voltage)
****************************************************/
byte* save_latest_position_eeprom(byte* addr)
{
	if (isConfigured(MODE_USE_ENCODER)) {
		eeprom_write_byte( addr++, hi(EncoderCount) );
		eeprom_write_byte( addr++, lo(EncoderCount) );
	}else {
		eeprom_write_byte( addr++, hi(PotSample[1]) );	
		eeprom_write_byte( addr++, lo(PotSample[1]) );
	}
	return addr;
}
/***************************************************
read_latest_position_eeprom()
Return			 :	The module configuration bytes
Also want to save:  Current thresholds.
Report DTCs (overtemperature, over voltage)
****************************************************/
byte* read_latest_position_eeprom(byte* addr)
{
	if (isConfigured(MODE_USE_ENCODER)) {
		EncoderCount = (eeprom_read_byte( addr++ )<<8);
		EncoderCount = (eeprom_read_byte( addr++ ));
	}else {
		PotSample[1] = (eeprom_read_byte( addr++ )<<8);
		PotSample[1] = (eeprom_read_byte( addr++ ));
	}
	return addr;
}

/************************************************************
getReportRate()		Read all EEPROM stored variables & constants.

RETURN	:  Rate as a multiple of 5ms.  ie Use to init a 
			counter in 5ms timeslice.
		   -1 means no Report desired!
*************************************************************/
byte getReportRate()  			
{
	switch ( (config_byte_2 & 0xF0) )
	{
	case MODE_SEND_UPDATES_NONE :	return 0; 	break;	
	case MODE_SEND_UPDATES_10ms :	return 2; 	break;
	case MODE_SEND_UPDATES_20ms :	return 4; 	break;
	case MODE_SEND_UPDATES_50ms :	return 10; 	break;
	case MODE_SEND_UPDATES_100ms:	return 20; break;
	default: return -1;  break;
	}
}	
