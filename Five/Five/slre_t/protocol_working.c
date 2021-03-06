#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"
#include "encoder.h"
#include "configuration.h"
#include "adc.h"
#include "serial.h"
#include "pwm.h"
#include "motor.h"



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
			printf("Match %d: %d %.*s\n", i, captures[i].len, captures[i].len, captures[i].ptr);		
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
		//printf("Match!\n");
		return 1;
	} 
	else 
		return 0;
}
/**********************************************************************/
/************************************************************************
	TRANSMISIONS:
 ************************************************************************/
char  Response[1024];
BOOL  ResponseReady = 0;
void form_response(const char* mText)
{	
	strcpy(Response, mText );
	ResponseReady = TRUE;
}

void send_positions()
{
	char buffer[255];
	if (FiveMotorConfigData.use_encoder) {
		sprintf(buffer,"Positions: v=%8ld, w=%8ld, x=%8ld, y=%8ld, z=%8ld", 
			Encoders[0].Count, Encoders[1].Count, Encoders[2].Count, Encoders[3].Count, Encoders[4].Count );
	} else {
		sprintf(buffer,"Positions: v=%8d, w=%8d, x=%8d, y=%8d, z=%8d", 
			PotSample[0], PotSample[1], PotSample[2], PotSample[3], PotSample[4] );
	}
	form_response( buffer );
}

void send_speeds()
{
	char buffer[255];
	if (FiveMotorConfigData.use_encoder) {
		sprintf(buffer,"Speeds (cps): v=%8ld, w=%8ld, x=%8ld, y=%8ld, z=%8ld", 
			Encoders[0].Speed, Encoders[1].Speed, Encoders[2].Speed, Encoders[3].Speed, Encoders[4].Speed );
	} else {
		sprintf(buffer,"Speeds (cps): v=%8d, w=%8d, x=%8d, y=%8d, z=%8d", 
			PotSample[0], PotSample[1], PotSample[2], PotSample[3], PotSample[4] );	
	}
	form_response( buffer );	
}

void send_base_frequency()
{
	char buffer[255];
	float num = get_base_frequency_herz();
	sprintf(buffer,"Base Frequency (hz): %6.2f", num );
	form_response(buffer);
}

/************************************************************************
	RECEPTIONS:
 ************************************************************************/
bool is_in_set(byte theSet, byte Bit)
{
	return ( theSet & (1<<Bit) );
}
BOOL parse_use_encoder( char* mIncoming )
{
	char expression[] = "^use (encoder|potentiometer)";
	int match = re_match( 2, Captures, expression, mIncoming );
	if (!match) return match;

	int use_encoder = strcmp(Captures[1].ptr, "encoder")==0;
	if (use_encoder) {
			FiveMotorConfigData.use_encoder = 1;
			form_response("use encoder");
	} else {
			FiveMotorConfigData.use_encoder = 0;
			form_response("using potentiometer");
	}	
	return match;
}

char* unit_to_string()
{
	static char retval[20];
	switch (FiveMotorConfigData.units)
	{
		case counts	: strcpy(retval, "counts");		break;
		case inch	: strcpy(retval, "inch"  );		break;
		case cm 	: strcpy(retval, "cm"	 );		break;
		case feet	: strcpy(retval, "feet"  );		break;											
		default 	: strcpy(retval, "unknown" );  	break;
	}
	return retval;
}

/* System Wide Unit Useage - ie all axis.
		Commands such as position will be interpreted in this unit.
		Commands such as speed will be interpreted in this unit per second.
		
		Possible to have different counts per unit however for different gearing 
*/
BOOL parse_set_unit( char* mIncoming )
{
	char expression[] = "^(set|read) unit ?(counts|inch|cm|feet)?";
	int match = re_match( 3, Captures, expression, mIncoming );
	if (match) {
		if (strncmp(Captures[1].ptr, "set", 3)==0) 	{
			if ( (strcmp(Captures[2].ptr, "counts")==0) )	{
				FiveMotorConfigData.units  = counts;   		// enum eMeasuringUnit		
				form_response( "unit=counts" );
			}
			if ( (strcmp(Captures[2].ptr, "inch")==0) )	{
				FiveMotorConfigData.units  = inch;	
				form_response( "unit=inch" );
			}
			if ( (strcmp(Captures[2].ptr, "cm")==0) )	{
				FiveMotorConfigData.units  = cm;   		// enum eMeasuringUnit		
				form_response( "unit=cm" );
			}
			if ( (strcmp(Captures[2].ptr, "feet")==0) )	 {
				FiveMotorConfigData.units  = feet;
				form_response( "unit=feet" );
			}
		} else {	// Read unit
			char buff[100];
			sprintf(buff, "unit= %s", unit_to_string() );
			form_response( buff );
		}
	}
	return match;
}

BOOL parse_gearing( char* mIncoming ) 
{
	char expression[] = "(set|read) (\\d+) counts_per_unit ?([vV])? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?";
	int  match = re_match( 2, Captures, expression, mIncoming );
	if (!match) return FALSE;

	char buff[100];		
	if ( (strcmp(Captures[1].ptr, "set")==0) )  {
		int counts = atoi( Captures[2].ptr );
		// V axis
		FiveMotorConfigData.v_counts_per_unit = counts;
		sprintf(buff, "%d counts per %s", counts, unit_to_string() );
		form_response( buff );
	} else {  // Read cpu
		sprintf(buff, "%ld counts per %s", FiveMotorConfigData.v_counts_per_unit, 
									unit_to_string() );		
		form_response( buff );	
	}
	return match;
}

BOOL parse_read_position( char* mIncoming )
{
	char expression[] = "read (position|speed|base frequency)";
	int  match = re_match( 2, Captures, expression, mIncoming );
	if (match) {
		BOOL send_pos   = strcmp(Captures[1].ptr, "position")==0;
		BOOL send_speed = strcmp(Captures[1].ptr, "speed")==0;
		BOOL send_frequency = strcmp(Captures[1].ptr, "base frequency")==0;
		if (send_pos)
		{	
			send_positions();	
			return TRUE;	
		} 
		if (send_speed)			
		{	
			send_speeds();		
			return TRUE;	
		} 
		if (send_frequency)
		{	
			send_base_frequency();		
			return TRUE;	
		}
	}
	return match;
}

BOOL parse_zero_encoders( char* mIncoming )
{
	char expression[] = "(zero|reset) (position|encoder) ?([vV])? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?";
	int match = re_match( 8, Captures, expression, mIncoming );
	if (match) {		
		printf("zeroing ");		
		for (int m=3; m<8; m++)
		{		
			if ((Captures[m].ptr) && (Captures[m].ptr)) {
				printf("%d, ", m );
				Encoders[m].Count=0;
			}
		}
		form_response( "zero positioned" );	
	}
	return match;
}

BOOL parse_measure_travel( char* mIncoming )
{
	char expression[] = "measure travel ?([vV])? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?";
	int match = re_match( 6, Captures, expression, mIncoming );
	if (match) {
		for (int m=1; m<6; m++)
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {
				set_motor_duty_letter( Captures[m].ptr[0], HOMING_SPEED );	
			}
		}
		// Don't wait until further limit switches triggered, just the cmd:
		form_response( "measuring travel..." );
	}
	return match;
}

BOOL parse_home_command( char* mIncoming )
{
	char expression[] = "home ([vV])? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?";
	int match = re_match( 6, Captures, expression, mIncoming );
	if (match) {
		for (int m=1; m<6; m++)
		{
			//printf("Match %d: %d %.*s\n", m, Captures[m].len, Captures[m].len, Captures[m].ptr);
			if ((Captures[m].ptr) && (Captures[m].len)) 
				set_motor_duty_letter( Captures[m].ptr[0], HOMING_SPEED );		
		}
		form_response("homing...");
	}
	return match;
}

BOOL parse_duty_command( char* mIncoming )
{
	char expression[] = "^(duty|pwm) ?([vV]\\d+)? ?([wW]\\d+)? ?([xX]\\d+)? ?([yY]\\d+)? ?([zZ]\\d+)?";
	int match = re_match( 7, Captures, expression, mIncoming );
	if (match) {
		//print_captures( 7, Captures);
		for (int m=2; m<=6; m++) 
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {
				const char* ptr   = Captures[m].ptr+1;
				float value = atof( ptr );
				set_motor_duty_letter( Captures[m].ptr[0], value );
			}
			form_response("duty request");
		}
	}
	return match;
}

BOOL parse_position_command( char* mIncoming )
{
	char expression[] = "^position ([vV]\\d+)? ?([wW]\\d+)? ?([xX]\\d+)? ?([yY]\\d+)? ?([zZ]\\d+)?";
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {
		for (int m=1; m<=6; m++)
		{		
			if ((Captures[m].ptr) && (Captures[m].len)) {
				const char* ptr   = Captures[m].ptr+1;
				float value = atof( ptr );			
				set_motor_position_letter( Captures[m].ptr[0], value );	
			}
		}
		form_response("position request");		
	}
	return match;
}

BOOL parse_speed_command( char* mIncoming )
{
	char expression[] = "^speed ([vV]\\d+)? ?([wW]\\d+)? ?([xX]\\d+)? ?([yY]\\d+)? ?([zZ]\\d+)?";
	int match = re_match( 6, Captures, expression, mIncoming );
	if (match) {
		for (int m=1; m<=6; m++)
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {
				float fraction = atof(Captures[m].ptr+1);
				set_motor_speed_letter( Captures[m].ptr[0], fraction );
			}
		}
		form_response("speed request");	
	}
	return match;
}

BOOL parse_limits_enable( char* mIncoming )
{
	char expression[] = "(enable|disable) limits ?[wW]? ?[vV]? ?[xX]? ?[yY]? ?[zZ]?";
	int match = re_match( 7, Captures, expression, mIncoming );
	if (match) {
		BOOL is_enable = (strcmp(Captures[1].ptr, "enable")==0);
		for (int m=2; m<=6; m++)
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {
				enable_limit( Captures[m].ptr[0],  is_enable);
			}
		}	
		form_response("limits enabled");
	}
	return match;
}

BOOL parse_firmware( char* mIncoming )
{
	char expression[] = "read (serial number|version)";
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {	
		BOOL is_sn       = strcmp(Captures[1].ptr, "serial number")==0;
		BOOL is_revision = strcmp(Captures[1].ptr, "version")==0;	
		char buff[100];		
		if (is_sn) {
			sprintf(buff, "Serial#=%ld", FiveMotorConfigData.serialNumber );	
			form_response(buff);
			return match;
		}
		if (is_revision)
			sprintf(buff,"Version=%d.%d.%d",
					 FiveMotorConfigData.FirmwareRevA,
					 FiveMotorConfigData.FirmwareRevB,
					 FiveMotorConfigData.FirmwareRevC );
			form_response(buff);
	}
	return match;
}

/************************************************************************
 MISCELLANEOUS COMMANDS : 
	Specialized instructions for 2 wheeled robot movements.
*************************************************************************/
BOOL parse_set_base_frequency_command( char* mIncoming )
{
	char buff[100];
	char expression[] = "set base frequency (\\d+)";
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {				
		float base_frequency = atof(Captures[1].ptr);
		sprintf(buff, "Base frequency= %6.2f\n", base_frequency);
		form_response( buff );
	}
	return match;
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
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {		
		float deg = atof(Captures[1].ptr);
		char buff[100];
		sprintf(buff, "Spinning %6.2f degrees", deg );
		form_response(buff);
	}
	return match;
}
BOOL parse_diff_move_command( char* mIncoming )
{
	char expression[] = "(forward|backward) (\\d+)";
	int match = re_match( 3, Captures, expression, mIncoming );
	char buff[100];
	if (match) {		
		BOOL is_forward = (strncmp(Captures[1].ptr, "forward", Captures[1].len)==0);
		float value = atof( Captures[2].ptr );
		if (is_forward==FALSE) value = -value;
		set_motor_position( 'v', value );
		set_motor_position( 'w', value );
		sprintf(buff, "moving %6.2f units", value );
		form_response(buff);
	}
	return match;
}

BOOL parse_diff_settings( char* mIncoming )
{
	char buff[100];
	char expression[] = "set (wheel diameter|wheel separation) (\\d+)";
	int match = re_match( 3, Captures, expression, mIncoming );
	if (match) {
		BOOL is_diameter = (strncmp(Captures[1].ptr, "wheel diameter", Captures[1].len )==0);
		if (is_diameter) {
			FiveMotorConfigData.wheel_diameter = atof( Captures[2].ptr );
			FiveMotorConfigData.wheel_circumference = M_PI * FiveMotorConfigData.wheel_diameter;			
			sprintf(buff, "wheel diameter=%6.1f", FiveMotorConfigData.wheel_diameter);
			form_response(buff);
		} else {		
			FiveMotorConfigData.wheel_separation = atof( Captures[2].ptr );
			sprintf(buff, "wheel separation=%6.1f", FiveMotorConfigData.wheel_separation);
			form_response(buff);			
		}
	}
	return match;
}

void test()
{
	top_level( "set 500 counts_per_unit v w x y z"  );
	top_level( "set 500 counts_per_unit x "  );	

	top_level( "zero encoder"		);
	top_level( "home v w x"			);
	top_level( "measure travel" 	);

	top_level( "pwm v101 w501 x201 y301 z401" 	);
	top_level( "pwm v100 x200 y300 z400" 	);
	top_level( "speed x200 y300 z400" 			);
	top_level( "speed v100 w500 x200 y300 z400" );	
	top_level( "position x0 y0 z0" 				);
	top_level( "position v100 w500 x200 y300 z400" );	

	top_level( "enable limits"  );
	top_level( "disable limits" );		

	// Test Differential Drive Commands:	
	top_level( "set counts_per_rev 500"  		);
	top_level( "set wheel diameter 6"      		);	// must be given in system set units
	top_level( "set wheel separation 18"	    );
		
	top_level( "spin 180 degrees" 		);
	top_level( "spin 360 degrees" 		);
	top_level( "forward 50 "  );
	top_level( "backward 5 " 	);
	
	top_level( "use encoder" 		);
	top_level( "use potentiometer"	);
	top_level( "set unit counts"	);	
	top_level( "set unit inch"		);	
	top_level( "set unit cm"		);
	top_level( "set unit feet"		);	top_level( "read unit" );

	top_level( "read position"		);
	top_level( "read speed" 		);
	top_level( "read base frequency");		
	top_level( "read serial number" );
	top_level( "read version"		);
}

/************************************************************************
 END Differential Drive Train Commands 
*************************************************************************/
void top_level( char* mIncoming )
{
	BOOL retval   = FALSE;
	*Response      = 0;
	ResponseReady = FALSE;
	
	printf( "%s\n", mIncoming );
	
	retval = parse_use_encoder			( mIncoming );		if (retval) goto EXIT;
	retval = parse_set_unit				( mIncoming );		if (retval) goto EXIT;
	retval = parse_gearing				( mIncoming );		if (retval) goto EXIT;
	retval = parse_zero_encoders		( mIncoming );		if (retval) goto EXIT;
	retval = parse_read_position		( mIncoming );		if (retval) goto EXIT;
	retval = parse_measure_travel		( mIncoming );		if (retval) goto EXIT;
	retval = parse_home_command			( mIncoming );		if (retval) goto EXIT;

	retval = parse_duty_command			( mIncoming );		if (retval) goto EXIT;
	retval = parse_position_command 	( mIncoming );		if (retval) goto EXIT;
	retval = parse_speed_command		( mIncoming );		if (retval) goto EXIT;

	retval = parse_limits_enable		( mIncoming );		if (retval) goto EXIT;
	retval = parse_firmware				( mIncoming );		if (retval) goto EXIT;
	
	retval = parse_set_base_frequency_command( mIncoming );	if (retval) goto EXIT;
	retval = parse_diff_spin_command		( mIncoming );	if (retval) goto EXIT;
	retval = parse_diff_move_command		( mIncoming );	if (retval) goto EXIT;
	retval = parse_diff_settings			( mIncoming );	if (retval) goto EXIT;
	if (retval==FALSE)
		form_response( "NAK:unknown command." );	
EXIT:
	if (ResponseReady) 
		printf( ">%s\n", Response );			
}

