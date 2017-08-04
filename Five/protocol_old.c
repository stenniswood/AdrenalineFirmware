#include <stdio.h>
#include <string.h>
#include "protocol.h"
#include "configuration.h"
#include "encoder.h"
#include "configuration.h"
#include "adc.h"
#include "serial.h"
#include "pwm.h"
#include "motor.h"
#include "slre.h"


struct cap	Captures[100+1];
void init_captures(int nPossCaptures, struct cap* captures)
{
	for (int i=0; i<=nPossCaptures; i++) {
		captures[i].len=0;
		captures[i].ptr=NULL;
	}
}

void print_captures(int nPossCaptures, struct cap* captures)
{
	printf("Request length: %d %s\n", captures[0].len, captures[0].ptr);
	for (int i=1; i<=nPossCaptures; i++)
		if (captures[i].ptr==NULL) 
			printf("Match %d:nf\n", i);
		else
			printf("Match %d: %.*s\n", i, captures[i].len, captures[i].ptr);		
}

int re_match( int nPossCaptures, struct cap* captures, char* mExpression, char* mString )
{
	struct slre	slre;
	int len    = strlen(mString);
	init_captures(nPossCaptures, captures);
	
	if (!slre_compile(&slre, mExpression )) {
		printf("Error compiling RE: %s\n", slre.err_str);	
		return 0;		
	}
	else if (slre_match(&slre, mString, len, captures)) {
		return 1;
	} 
	else 
		return 0;
}


bool is_in_set(byte theSet, byte Bit)
{
	return ( theSet & (1<<Bit) );
}

byte which_motors(char* mMessage)
{
}

char  Response[1024];
BOOL  ResponseReady = 0;
void form_response(const char* mText)
{
	sprintf(Response, mText );
	ResponseReady = TRUE;
}

BOOL parse_use_encoder( char* mIncoming )
{
	char expression[] = "^use (encoder|potentiometer)";
	int match = re_match( 2, Captures, expression, mIncoming );
		
	int use_encoder 	   = strcmp(mIncoming, "use encoder");
	int use_potentiometer  = strcmp(mIncoming, "use potientiometer");			
	if (use_encoder==0) {
			FiveMotorConfigData.use_encoder = 1;
			form_response("ACK: use encoder");
	} else if (use_potentiometer==0) {
			FiveMotorConfigData.use_encoder = 0;
			form_response("ACK using potentiometer");
	}
	return FALSE;
}

BOOL parse_set_unit( char* mIncoming )
{
	char expression = "^set unit (inch|meter|mm|feet)";
	int match = re_match( 2, Captures, expression, mIncoming );
	
	int set_unit = strcmp(mIncoming, "set unit ");
	char* ptr = mIncoming+0;
	if ( (strcmp(mIncoming, "meters")==0) )	{
		FiveMotorConfigData.units  = meters;   		// enum eMeasuringUnit		
		form_response( "ACK unit=meters" );
	} if ( (strcmp(mIncoming, "mm")==0) )	{
		FiveMotorConfigData.units  = millimeters;   // enum eMeasuringUnit
		form_response( "ACK unit=mm" );
	} if ( (strcmp(mIncoming, "feet")==0) )	 {
		FiveMotorConfigData.units  = feet;
		form_response( "ACK unit=feet" );
	} if ( (strcmp(mIncoming, "inches")==0) )	{
		FiveMotorConfigData.units  = inches;	
		form_response( "ACK unit=inches" );
	}
	return FALSE;
}

BOOL parse_read_position( char* mIncoming )
{
	char expression = "read (position|speed|frequency)";
	int match = re_match( 2, Captures, expression, mIncoming );
		
	BOOL send_pos   = strcmp(mIncoming, "read position ");
	BOOL send_speed = strcmp(mIncoming, "read speed ");
	BOOL send_base_frequency = strcmp(mIncoming, "read frequency ");	
	if (send_pos) 
	{	
		send_positions();	
		return TRUE;	
	} if (send_speed)			
	{	
		send_speeds();		
		return TRUE;	
	} if (send_base_frequency)
	{	
		send_speeds();		
		return TRUE;	
	}
	return FALSE;
}

BOOL parse_zero_encoders( char* mIncoming )
{
	char expression = "zero (position|encoders) (v)?(w)?(x)?(y)?(z)?";
	int match = re_match( 7, Captures, expression, mIncoming );
		
	BOOL is_speed_command = strcmp(mIncoming, "zero position ");

	for (int b=0; b<NUM_MOTORS; b++)
	{
		if (is_in_set(which, b) )
			Encoders[b].Count=0;
	}
	form_response( "ACK zero positioned" );	
	return is_speed_command;
}

BOOL parse_measure_travel( char* mIncoming )
{
	char expression = "measure travel";
	int match = re_match( 2, Captures, expression, mIncoming );
	
	BOOL is_speed_command = strcmp(mIncoming, "measure travel");
	byte which = which_motors(mIncoming);
	for (int b=0; b<NUM_MOTORS; b++)
	{
		if (is_in_set(which, b) )
			set_motor_duty( b, HOMING_SPEED );		
	}
	// Don't wait until further limit switches triggered, just ack the cmd:
	form_response( "ACK measuring travel..." );
	return is_speed_command;
}

BOOL parse_home_command( char* mIncoming )
{
	char expression = "home ([vV])? ([wW])? ([xX])? ([yY])? ([zZ])?";
	int match = re_match( 2, Captures, expression, mIncoming );
		
	BOOL is_speed_command = strcmp(mIncoming, "HOME:");
	byte which = which_motors(mIncoming);
	for (int b=0; b<NUM_MOTORS; b++)
	{
		if (is_in_set(which, b) )
			set_motor_duty( b, HOMING_SPEED );		
	}
	form_response("ACK: homing...");
	return is_speed_command;
}

BOOL parse_duty_command( char* mIncoming )
{
	char expression[] = "^motor duty:([vV]\\d+)? ?([wW]\\d+)? ?([xX]\\d+)? ?([yY]\\d+)? ?([zZ]\\d+)?";
	
	BOOL is_speed_command = strcmp(mIncoming, "motor duty");
	byte which = which_motors(mIncoming);
	float fraction=0.0;
	for (int b=0; b<NUM_MOTORS; b++)
	{
		if (is_in_set(which, b) )
			set_motor_duty( b, fraction );		
	}
	form_response("ACK: speed request");	
	return is_speed_command;
}
BOOL parse_position_command( char* mIncoming )
{
	char expression[] = "^motor position:([vV]\\d+)? ?([wW]\\d+)? ?([xX]\\d+)? ?([yY]\\d+)? ?([zZ]\\d+)?";
	
	BOOL is_position_command = strcmp(mIncoming, "motor position ");
	byte which = which_motors(mIncoming);
	uint32_t pos;
	for (int b=0; b<NUM_MOTORS; b++)
	{
		if (is_in_set(which, b) )
			set_motor_position( b, pos );		
	}	
	form_response("ACK: position request");		
	return is_position_command;
}
BOOL parse_speed_command( char* mIncoming )
{
	char expression[] = "^motor speed:([vV]\\d+)? ?([wW]\\d+)? ?([xX]\\d+)? ?([yY]\\d+)? ?([zZ]\\d+)?";
	
	BOOL is_speed_command = strcmp(mIncoming, "motor speed");
	byte which = which_motors(mIncoming);
	float fraction=0.0;
	for (int b=0; b<NUM_MOTORS; b++)
	{
		if (is_in_set(which, b) )
			set_motor_duty( b, fraction );		
	}
	form_response("ACK: speed request");	
	return is_speed_command;
}

BOOL parse_limits_enable( char* mIncoming )
{
	char expression[] = "(enable|disable) limits";
	
	BOOL is_position_command = strcmp(mIncoming, "enable limits");
	is_position_command = strcmp(mIncoming, "disable limits");	
	byte which = which_motors(mIncoming);
	uint32_t pos;
	for (int b=0; b<NUM_MOTORS; b++)
	{
		if (is_in_set(which, b) )
			set_motor_position( b, pos );		
	}	
	form_response("ACK: limits enabled");
	return is_position_command;
}

BOOL parse_firmware( char* mIncoming )
{
	char expression[] = "read (serial number|revision)";

	BOOL is_position_command = strcmp(mIncoming, "get serial number");
	sprintf(Response, "Serial#=%ld", FiveMotorConfigData.serialNumber );	
	form_response(Response);
	BOOL is_revision = strcmp(mIncoming, "revision");	
	if (is_revision)
		sprintf(Response,"Version=%d.%d.%d",
				 FiveMotorConfigData.FirmwareRevA,
				 FiveMotorConfigData.FirmwareRevB,
				 FiveMotorConfigData.FirmwareRevC );
		form_response(Response);
	return is_position_command;
}

/************************************************************************
 MISCELLANEOUS COMMANDS : 
	Specialized instructions for 2 wheeled robot movements.
*************************************************************************/
BOOL parse_set_base_frequency_command( char* mIncoming )
{
	char expression[] = "set base frequency (\\d+)";
	
	BOOL is_position_command = strcmp(mIncoming, "set base frequency:");

	return is_position_command;
}
/************************************************************************
 END MISCELLANEOUS COMMANDS.
*************************************************************************/


/************************************************************************
 Differential Drive Train Commands 
	Specialized instructions for 2 wheeled robot movements.
*************************************************************************/
BOOL parse_diff_spin_command( char* mIncoming )
{
	char expression[] = "spin (\\d+) degrees";
	
	BOOL is_position_command = strcmp(mIncoming, "spin %d degrees");
	return is_position_command;
}
BOOL parse_diff_move_command( char* mIncoming )
{
	char expression[] = "(forward|backward) distance (\\d+)";
	
	BOOL is_position_command = strcmp(mIncoming, "forward distance ");
	is_position_command = strcmp(mIncoming, "backward distance ");	
//	BOOL is_position_command = strcmp(mIncoming, "forward speed ");
//	BOOL is_position_command = strcmp(mIncoming, "backward speed ");
	return is_position_command;
}

void test()
{
	top_level( "use encoder" );
	top_level( "use potentiometer" );
	top_level( "reset encoder" );
	top_level( "read position" );
	top_level( "read speed" );	
	top_level( "home v w x" );
	top_level( "measure travel" );
			
	top_level( "set units meters" );
	top_level( "use unit inch" );
	top_level( "use unit feet" );	

	top_level( "motorspeed:x200 y300 z400" );
	top_level( "motorspeed:v100 w500 x200 y300 z400" );	
	top_level( "pwm:v100 w500 x200 y300 z400" );
	top_level( "pwm:v100 w500 x200 y300 z400" );	
	

	top_level( "spin 180 degrrees" );		
	top_level( "spin 360 degrrees" );
	top_level( "move forward 50" );		
	top_level( "move forward 50" );			
	
}

/************************************************************************
 END Differential Drive Train Commands 
*************************************************************************/
void top_level( char* mIncoming )
{
	bool retval=false;

	retval = parse_use_encoder			( mIncoming );		if (retval) return;
	retval = parse_set_unit				( mIncoming );		if (retval) return;
	retval = parse_zero_encoders		( mIncoming );		if (retval) return;
	retval = parse_read_position		( mIncoming );		if (retval) return;
	retval = parse_measure_travel		( mIncoming );		if (retval) return;
	retval = parse_home_command			( mIncoming );		if (retval) return;

	retval = parse_duty_command			( mIncoming );		if (retval) return;
	retval = parse_position_command 	( mIncoming );		if (retval) return;
	retval = parse_speed_command		( mIncoming );		if (retval) return;

	retval = parse_limits_enable		( mIncoming );		if (retval) return;
	retval = parse_firmware				( mIncoming );		if (retval) return;
	
	retval = parse_set_base_frequency_command( mIncoming );	if (retval) return;
	retval = parse_diff_spin_command		( mIncoming );	if (retval) return;
	retval = parse_diff_move_command		( mIncoming );	if (retval) return;
	
	if (retval==false)
		form_response( "NAK:unknown command." );		
}

/************************************************************************
	TRANSMISIONS:
 ************************************************************************/
void send_positions()
{
	char buffer[255];
	if (FiveMotorConfigData.use_encoder) {
		sprintf(buffer,"Positions: v=%8ld, w=%8ld, x=%8ld, y=%8ld, z=%8ld\n", 
			Encoders[0].Count, Encoders[1].Count, Encoders[2].Count, Encoders[3].Count, Encoders[4].Count );
	} else {
		sprintf(buffer,"Positions: v=%8d, w=%8d, x=%8d, y=%8d, z=%8d\n", 
			PotSample[0], PotSample[1], PotSample[2], PotSample[3], PotSample[3], PotSample[4] );
	}
	send_message(buffer);
}

void send_speeds()
{
	char buffer[255];
	if (FiveMotorConfigData.use_encoder) {
		sprintf(buffer,"Speeds (cps): v=%8ld, w=%8ld, x=%8ld, y=%8ld, z=%8ld\n", 
			Encoders[0].Speed, Encoders[0].Speed, Encoders[0].Speed, Encoders[0].Speed );
	} else {
		sprintf(buffer,"Speeds (cps): v=%8d, w=%8d, x=%8d, y=%8d, z=%8d\n", 
			PotSample[0], PotSample[1], PotSample[2], PotSample[3] );	
	}
	send_message(buffer);	
}

void send_base_frequency()
{
	char buffer[255];
	float num = get_base_frequency_herz();
	sprintf(buffer,"Base Frequency (hz): %6.2f\n", num );
	form_response(buffer);
	send_message (buffer);	
}

