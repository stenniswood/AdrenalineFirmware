/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Low Level Routines RTOS					 	 	 *
 *											  						 *
 *	  CATEGORY	 :   BIOS Driver									 *
 *											   						 *
 *    PURPOSE	 : Save and Recall settings from EEPROM.			 *
 *																	 *
 *	  HARDWARE RESOURCES:									   		 *
 *			 Atmega16m1 EEPROM								 		 *
 *																	 *
 *	  EEPROM DATA:													 *
 *			MyInstance												 *
 *			EndPoint1 (value & angle)								 *
 *			EndPoint2 (value & angle)								 *
 *			config_byte_1											 *
 *			config_byte_2											 *
 *			Motor Stall Current 1									 *
 *			Motor Stall Current 2									 *	
 *			Latest Position (EncoderCount)							 *
 *											   						 *
 *    AUTHOR: Stephen Tenniswood, Robotics Engineer 	 			 *
 *		  	  November 2013						 				 	 *
 *			  Product of Beyond Kinetics, Inc						 *
 *		  	  Revised for atmel Sept 2013						 	 * 
 *********************************************************************/
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

/*******************************************************
save_stops_eeprom()		

Save both structures EndPoint1 & EndPoint2.
RETURN:		Last address written + 1 (ie. next available
			EEPROM address).
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
	current_1_threshold = ((eeprom_read_byte( addr+2 ))<<2);
	current_2_threshold = ((eeprom_read_byte( addr+3 ))<<2);
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
		eeprom_write_byte( addr++, hi(EncoderCount) );
		eeprom_write_byte( addr++, lo(EncoderCount) );
	}else {
		eeprom_write_byte( addr++, hi(PotSample[1]) );	
		eeprom_write_byte( addr++, lo(PotSample[1]) );
	}
	return addr;
}
/**********************************************************
save_cal()		Save all EEPROM stored variables & constants.

Saves to EEPROM 
***********************************************************/
byte* save_cal(byte* addr)
{
	//byte* addr = CONFIGURATION_EEPROM_ADDRESS;
	//save_instance_eeprom();
	addr = save_stops_eeprom			(addr);
	addr = save_configuration_eeprom	(addr);
	addr = save_currentThresholds_eeprom(addr);
	addr = save_latest_position_eeprom (addr);
	return addr;
};

/************************************************************
read_cal()		Read all EEPROM stored variables & constants.

Read from EEPROM, and stores into the RAM
*************************************************************/
byte* read_cal(byte* addr)
{
	//byte* addr = CONFIGURATION_EEPROM_ADDRESS;
	addr = read_stops_eeprom			(addr);
	addr = read_configuration_eeprom	(addr);
	addr = read_currentThresholds_eeprom(addr);
	addr = read_latest_position_eeprom	(addr);
	return addr;	
};

/************************************************************
read_cal()		Read all EEPROM stored variables & constants.

RETURN	:  Rate as a multiple of 5ms.  ie Use to init a 
			counter in 5ms timeslice.
		   -1 means no Report desired!
*************************************************************/
byte getReportRate()  			
{  
	switch ( (config_byte_2 & 0xF0) )
	{
	case MODE_SEND_UPDATES_NONE :	return -1; 	break;	
	case MODE_SEND_UPDATES_10ms :	return 2; 	break;
	case MODE_SEND_UPDATES_20ms :	return 4; 	break;
	case MODE_SEND_UPDATES_50ms :	return 10; 	break;
	case MODE_SEND_UPDATES_100ms:	return 20; 	break;
	default: return -1;  break;
	}
}	


void mark_stop_1( float mAngle ){};
void mark_stop_2( float mAngle ){};


