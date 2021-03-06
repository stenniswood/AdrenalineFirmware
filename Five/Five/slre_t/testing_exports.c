#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "encoder.h"
#include "testing_exports.h"


char   motor_letters[NUM_MOTORS] = { 'v','w','x','y','z' };

float 	 CurrentSampleAvg[5] = { 0.0, 0.0, 0.0, 0.0, 0.0 };

/**************** ENCODER EXTERN FOR TESTING PROTOCOL **********************************/
void  set_motor_position   ( int mIndex, uint32_t Position ) { printf("mp %d=%ld", mIndex, Position ); 	  };
void  set_motor_duty_letter( char Letter, float mFraction )  { printf("md %c=%6.1f", Letter, mFraction ); };
void  set_motor_speed_letter( char Letter, float mFraction ) { printf("ms %c=%6.1f", Letter, mFraction ); };
float get_base_frequency_herz( ) 							{ return 100; };

uint16_t PotSample[5];

struct stEncoder Encoders[NUM_MOTORS];
struct stConfigData FiveMotorConfigData;

/****************CONFIGURATION EXTERN FOR TESTING PROTOCOL **********************************/
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

void set_motor_position_letter( char Letter, uint32_t Position )
{
	printf("mp %c=%ld", Letter, Position ); 
	switch( Letter )
	{
/*	case 'v': set_motor_position_1(  Position );	break;
	case 'w': set_motor_position_2(  Position );	break;
	case 'x': set_motor_position_3(  Position );	break;
	case 'y': set_motor_position_4(  Position );	break;
	case 'z': set_motor_position_5(  Position );	break; */
	default : break;
	}
}

void read_configuration()
{
	unsigned int  Address = 0;
	
	int size = sizeof( struct stConfigData );
	unsigned char* ptr = (unsigned char*)&FiveMotorConfigData;	
	for (int i=0; i<size; i++) 
	{
//		*ptr = EEPROM_read( Address++ );
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
//		EEPROM_write( Address++,  *ptr );
		ptr++;		
	}
}
void stop_motors()
{
	// Don't Store in MotDuties[] for resume purposes.
}

void send_prompt( )
{

}

void set_base_PWM_freq( float mHerz )
{
}

char *dtostrf (double val, signed char width, unsigned char prec, char *sout)
{
	char formatter[10];
	sprintf(formatter, "%%d.%sf", width, prec);
	sprintf(sout, formatter, val);

    return sout;
}

BOOL send_message( char* mMsg )
{
	printf( "%s", (char*) mMsg );
}