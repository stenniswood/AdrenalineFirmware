#include <math.h>
#include <stdlib.h>
//#include <stdio.h>
#include "configuration.h"
#include "eeprom.h"
#include "pwm.h"
#include "motor.h"


struct stConfigData FiveMotorConfigData;


void init_configuration()
{
	FiveMotorConfigData.use_encoder    = TRUE;	
	FiveMotorConfigData.base_frequency = 500.0;
	FiveMotorConfigData.units		   = inch;

	FiveMotorConfigData.wheel_separation = 24;
	FiveMotorConfigData.wheel_diameter   = 9;	
	FiveMotorConfigData.wheel_circumference = FiveMotorConfigData.wheel_diameter*M_PI;		// calculated quantity
	FiveMotorConfigData.wheel_counts_per_rev = 360;		// other variables kept in sync with this.	

	FiveMotorConfigData.encoders_saved  = TRUE;	
	for (int i=0; i<5; i++) 
	{
		FiveMotorConfigData.use_limits[i]  		= TRUE; 	
		FiveMotorConfigData.counts_per_unit[i] 	= 500;
		FiveMotorConfigData.motor_status[i] 	= 0;
		FiveMotorConfigData.Kp[i]  				= 1.0;
		FiveMotorConfigData.Ki[i]  				= 0.0;
	}

	FiveMotorConfigData.serialNumber = 123456;
	FiveMotorConfigData.FirmwareRevA = 1;
	FiveMotorConfigData.FirmwareRevB = 0;
	FiveMotorConfigData.FirmwareRevC = 0;
}


void read_configuration()
{
	unsigned int  Address = 0;
	
	int size = sizeof( struct stConfigData );
	unsigned char* ptr = (unsigned char*)&FiveMotorConfigData;	
	for (int i=0; i<size; i++) 
	{
		*ptr = EEPROM_read( Address++ );
		ptr++;		
	}
}

void save_encoders()
{
	unsigned int  Address = 0;
	char* ptr = Encoders[0].Count;
	for (int i=0; i<NUM_MOTORS; i++) 
	{
		ptr = &(Encoders[i].Count) ;
		*ptr = EEPROM_read( Address++ );		ptr++;
		*ptr = EEPROM_read( Address++ );		ptr++;				
		*ptr = EEPROM_read( Address++ );		ptr++;
		*ptr = EEPROM_read( Address++ );		ptr++;
	}
}

void read_encoders()
{
	unsigned int  Address = 0;
	char* ptr = Encoders[0].Count;
	for (int i=0; i<NUM_MOTORS; i++) 
	{
		ptr = &(Encoders[i].Count) ;
		EEPROM_write( Address++,  *ptr );		ptr++;
		EEPROM_write( Address++,  *ptr );		ptr++;				
		EEPROM_write( Address++,  *ptr );		ptr++;
		EEPROM_write( Address++,  *ptr );		ptr++;
	}

}

void save_configuration()
{
	unsigned int  Address = 0;
	
	int size = sizeof( struct stConfigData );
	unsigned char* ptr = (unsigned char*)&FiveMotorConfigData;	
	for (int i=0; i<size; i++) 
	{
		EEPROM_write( Address++,  *ptr );
		ptr++;		
	}
}


void read_configuration_and_set()
{
	read_configuration();
	set_base_PWM_freq( FiveMotorConfigData.base_frequency );
	for (int i=0; i<5; i++)
		set_motor_duty_letter(motor_letters[i], mot_states[i].duty_fraction );
}

