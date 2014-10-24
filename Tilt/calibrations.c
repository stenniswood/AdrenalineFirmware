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
#include "configuration.h"
#include "calibrations.h"


/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/

/* This is called after system default configuration. 
	Only 1 time for the life of the board (or until it is reflashed)  
	To set the application defaults.		*/
void app_default_configuration()
{
	config_byte_2 = MODE_SEND_UPDATES_30ms;
	config_byte_2 |= (MODE_SEND_ACCEL | MODE_SEND_GYRO | MODE_SEND_MAGNET);
	config_byte_3 |= (MODE_ACCEL_2g | MAGNET_DATA_RATE_30 | MAGNET_AVGS_ONE);	
}

byte getAccelSensitivity() { return (config_byte_3 & MODE_ACCEL_SENSITIVITY_MASK);	}
byte getMagnetDataRate()   { return (config_byte_3 & MAGNET_DATA_RATE_MASK); 		}
byte getMagnetAvgs()   	   { return (config_byte_3 & MAGNET_AVGS_MASK);		 		}
byte getTemperatureEnable() { return (config_byte_3 & MODE_READ_TEMPERATURE);		}


/**********************************************************
save_cal()		Save all EEPROM stored variables & constants.

Saves to EEPROM 
***********************************************************/
byte* save_cal()
{
	byte* addr;
	addr = save_configuration();
	return addr;
};

/************************************************************
read_cal()		Read all EEPROM stored variables & constants.

Read from EEPROM, and stores into the RAM
*************************************************************/
byte* read_cal()
{
	byte* addr;
	addr = read_configuration();
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
	byte num_10_ms =  ((config_byte_2 & 0xF0)>>4);
	return num_10_ms;
}	


