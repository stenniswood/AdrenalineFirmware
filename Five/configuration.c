#include <math.h>
#include <stdlib.h>
//#include <stdio.h>

#include "configuration.h"
#include "eeprom.h"


struct stConfigData FiveMotorConfigData;


void init_configuration()
{
	FiveMotorConfigData.use_encoder = TRUE;	
	FiveMotorConfigData.use_limits[0]  = TRUE; 
	FiveMotorConfigData.use_limits[1]  = TRUE; 
	FiveMotorConfigData.use_limits[2]  = TRUE; 
	FiveMotorConfigData.use_limits[3]  = TRUE; 
	FiveMotorConfigData.use_limits[4]  = TRUE; 
	
	FiveMotorConfigData.base_frequency = 500.0;
	FiveMotorConfigData.units		   = inch;
	FiveMotorConfigData.counts_per_unit[0] = 500;
	FiveMotorConfigData.counts_per_unit[1] = 500;
	FiveMotorConfigData.counts_per_unit[2] = 500;
	FiveMotorConfigData.counts_per_unit[3] = 500;
	FiveMotorConfigData.counts_per_unit[4] = 500;

	FiveMotorConfigData.wheel_separation = 24;
	FiveMotorConfigData.wheel_diameter   = 9;	
	FiveMotorConfigData.wheel_circumference = FiveMotorConfigData.wheel_diameter*M_PI;		// calculated quantity

	FiveMotorConfigData.encoders_saved  = TRUE;	
	FiveMotorConfigData.motor_status[0] = 0;
	FiveMotorConfigData.motor_status[1] = 0;
	FiveMotorConfigData.motor_status[2] = 0;
	FiveMotorConfigData.motor_status[3] = 0;
	FiveMotorConfigData.motor_status[4] = 0;	
	
		
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

void  enable_limit( char Letter, BOOL mEnable ) 
{ 
/*	if (mEnable)
		printf("enabling Limit %c\n", Letter);
	else 
		printf("disabling Limit %c\n", Letter);*/
		
	switch (Letter)
	{
	case 'v' : FiveMotorConfigData.use_limits[0]=mEnable; 	break;
	case 'w' : FiveMotorConfigData.use_limits[1]=mEnable; 	break;
	case 'x' : FiveMotorConfigData.use_limits[2]=mEnable; 	break;
	case 'y' : FiveMotorConfigData.use_limits[3]=mEnable; 	break;
	case 'z' : FiveMotorConfigData.use_limits[4]=mEnable; 	break;
	default	 : break;
	}
};

