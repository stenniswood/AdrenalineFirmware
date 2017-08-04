#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "testing_exports.h"


/**************** ENCODER EXTERN FOR TESTING PROTOCOL **********************************/
void  set_motor_position   ( int mIndex, uint32_t Position ) { printf("mp %d=%ld", mIndex, Position ); 	  };
BOOL  send_message   	   ( char* mMessage )				 { return 0;   };
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
	FiveMotorConfigData.v_use_limits = TRUE;
	FiveMotorConfigData.w_use_limits = TRUE;
	FiveMotorConfigData.x_use_limits = TRUE;
	FiveMotorConfigData.y_use_limits = TRUE;
	FiveMotorConfigData.z_use_limits = TRUE;

	FiveMotorConfigData.units		= inch;
	FiveMotorConfigData.v_counts_per_unit = 500;
	FiveMotorConfigData.w_counts_per_unit = 500;
	FiveMotorConfigData.x_counts_per_unit = 500;
	FiveMotorConfigData.y_counts_per_unit = 500;
	FiveMotorConfigData.z_counts_per_unit = 500;
	
	FiveMotorConfigData.wheel_separation = 24;
	FiveMotorConfigData.wheel_diameter	 = 6;	
	FiveMotorConfigData.wheel_circumference=M_PI*FiveMotorConfigData.wheel_diameter;
				
	FiveMotorConfigData.serialNumber = 1234566789;
	FiveMotorConfigData.FirmwareRevA = 1;
	FiveMotorConfigData.FirmwareRevB = 0;
	FiveMotorConfigData.FirmwareRevC = 0;	
}

void  enable_limit( char Letter, BOOL mEnable ) 
{ 
	if (mEnable)
		printf("enabling Limit %c\n", Letter);
	else 
		printf("disabling Limit %c\n", Letter);
		
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


