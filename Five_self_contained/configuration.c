#include <math.h>
#include <stdlib.h>
//#include <stdio.h>

#include "configuration.h"
#include "eeprom.h"


struct stConfigData FiveMotorConfigData;


void init_configuration()
{
	FiveMotorConfigData.use_encoder = TRUE;	
	FiveMotorConfigData.v_use_limits  = TRUE; 
	FiveMotorConfigData.w_use_limits  = TRUE; 
	FiveMotorConfigData.x_use_limits  = TRUE; 
	FiveMotorConfigData.y_use_limits  = TRUE; 
	FiveMotorConfigData.z_use_limits  = TRUE; 
	
	FiveMotorConfigData.units		= inch;
	FiveMotorConfigData.v_counts_per_unit = 500;
	FiveMotorConfigData.w_counts_per_unit = 500;
	FiveMotorConfigData.x_counts_per_unit = 500;
	FiveMotorConfigData.y_counts_per_unit = 500;
	FiveMotorConfigData.z_counts_per_unit = 500;
	
	FiveMotorConfigData.wheel_separation = 24;
	FiveMotorConfigData.wheel_diameter   = 12;	
	FiveMotorConfigData.wheel_circumference = 12*M_PI;		// calculated quantity
		
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
	case 'v' : FiveMotorConfigData.v_use_limits=mEnable; 	break;
	case 'w' : FiveMotorConfigData.w_use_limits=mEnable; 	break;
	case 'x' : FiveMotorConfigData.x_use_limits=mEnable; 	break;
	case 'y' : FiveMotorConfigData.y_use_limits=mEnable; 	break;
	case 'z' : FiveMotorConfigData.z_use_limits=mEnable; 	break;
	default	 : break;
	}
};

