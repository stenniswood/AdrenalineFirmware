#include <stdio.h>
#include <string.h>
#include "protocol.h"
#include "configuration.h"
#include "encoder.h"
#include "configuration.h"
#include "adc.h"
#include "serial.h"
#include "pwm.h"


BOOL parse_encoder_string( char* mIncoming )
{
	int use_encoder = strcmp(mIncoming, "use encoder");
	int use_potentiometer  = strcmp(mIncoming, "use potientiometer");	
		
	if ((use_potentiometer==0) || (use_encoder==0))
		QuadMotorConfigData.use_encoder = (use_encoder==0);
	return FALSE;
}

BOOL parse_speed_command( char* mIncoming )
{
	BOOL is_speed_command = strcmp(mIncoming, "MOTOR_SPEED:");
	return is_speed_command;
}

BOOL parse_position_command( char* mIncoming )
{
	BOOL is_position_command = strcmp(mIncoming, "MOTOR_POSITION:");
	return is_position_command;
}

BOOL parse_set_base_frequency_command( char* mIncoming )
{
	BOOL is_position_command = strcmp(mIncoming, "set base frequency:");

	return is_position_command;
}


void top_level( char* mIncoming )
{
	bool retval = parse_encoder_string	( mIncoming );
	retval = parse_speed_command		( mIncoming );
	retval = parse_position_command 	( mIncoming );
	retval = parse_set_base_frequency_command( mIncoming );

	int read_encoder  = strcmp(mIncoming, "read position");	
	
}

/************************************************************************
	TRANSMISIONS:
 ************************************************************************/
 
void send_positions()
{
	char buffer[255];
	if (QuadMotorConfigData.use_encoder) {
		sprintf(buffer,"Positions: M1=%8ld, M2=%8ld, M3=%8ld, M4=%8ld\n", 
			EncoderCount[0], EncoderCount[1], EncoderCount[2], EncoderCount[3] );
	} else {
		sprintf(buffer,"Positions: M1=%8d, M2=%8d, M3=%8d, M4=%8d\n", 
			PotSample[0], PotSample[1], PotSample[2], PotSample[3] );	
	}
	send_message(buffer);
}

void send_speeds()
{
	char buffer[255];
	if (QuadMotorConfigData.use_encoder) {
		sprintf(buffer,"Speeds (cps): M1=%8ld, M2=%8ld, M3=%8ld, M4=%8ld\n", 
			EncoderSpeed[0], EncoderSpeed[1], EncoderSpeed[2], EncoderSpeed[3] );
	} else {
		sprintf(buffer,"Speeds (cps): M1=%8d, M2=%8d, M3=%8d, M4=%8d\n", 
			PotSample[0], PotSample[1], PotSample[2], PotSample[3] );	
	}
	send_message(buffer);	
}


void send_base_frequency()
{
	char buffer[255];
	if (QuadMotorConfigData.use_encoder) {
		float num = get_base_frequency_herz();
		sprintf(buffer,"Base Frequency (hz): %6.2f\n", num );
	} else {
		
	}
	send_message(buffer);	
}

