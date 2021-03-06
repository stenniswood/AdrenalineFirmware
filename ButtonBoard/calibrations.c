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
#include "can.h"
#include "can_instance.h"
#include "pwm.h"
#include "eeprom.h"
#include "calibrations.h"
#include "configuration.h"


/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/

/*******************************************************
   This is called after system default configuration. 
	Only 1 time for the life of the board (or until it is reflashed)  
	To set the application defaults.		
 *******************************************************/	
void app_default_configuration()
{
	config_byte_1 = 0;
	config_byte_2 = 0;
}

/* Callback for configuration change. */
void config_change( byte mByteChanged )
{
	save_cal();	
}


/**********************************************************
save_cal()		Save all EEPROM stored variables & constants.

Saves to EEPROM 
***********************************************************/
byte* save_cal()
{
	byte* addr = save_configuration();
	addr = save_configuration_eeprom	(addr);
	return addr;
};

/************************************************************
read_cal()		Read all EEPROM stored variables & constants.

Read from EEPROM, and stores into the RAM
*************************************************************/
byte* read_cal()
{
	byte* addr = read_configuration();
	addr = read_configuration_eeprom	(addr);
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


