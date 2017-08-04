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
	strcat(Response, "\r\n");
	ResponseReady = TRUE;
}

void send_current_readings()
{
	char buffer[255];
	char temp[10];
		
	sprintf(buffer, "Current: ");
	for (int i=0; i<5; i++)
	{
		dtostrf( CurrentSampleAvg[i], 3, 1, temp );
		strcat( buffer, temp );
		strcat( buffer, " ");
	}	
	form_response( buffer );
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
	char temp[10];
	dtostrf( num, 4, 1, temp );
	sprintf(buffer,"Base Frequency (hz): %s", temp );
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

	char* use_encoder = strstr(Captures[1].ptr, "encoder");
	if (use_encoder) {
			FiveMotorConfigData.use_encoder = 1;
			form_response("using encoder");
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
		case meter 	: strcpy(retval, "meter" );		break;
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
	char expression[] = "^(set|read) unit ?(counts|inch|meter|feet)?";
	int match = re_match( 3, Captures, expression, mIncoming );
	if (match) {
		if (strstr(Captures[1].ptr, "set")!=NULL) 	{
			if ( (strstr(Captures[2].ptr, "counts")!=NULL) )	{
				FiveMotorConfigData.units  = counts;   		// enum eMeasuringUnit		
				form_response( "unit=counts" );
			}
			if ( (strstr(Captures[2].ptr, "inch")!=NULL) )	{
				FiveMotorConfigData.units  = inch;	
				form_response( "unit=inch" );
			}
			if ( (strstr(Captures[2].ptr, "meter")!=NULL) )	{
				FiveMotorConfigData.units  = meter;   		// enum eMeasuringUnit		
				form_response( "unit=meter" );
			}
			if ( (strstr(Captures[2].ptr, "feet")!=NULL) )	 {
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
	if ( (strstr(Captures[1].ptr, "set")!=NULL) )  {
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
	char expression[] = "read (position|speed|current|status|robot info|base frequency)";
	int  match = re_match( 2, Captures, expression, mIncoming );
	if (match) {
		char* send_status   = strstr(Captures[1].ptr, "status");
		char* send_robot    = strstr(Captures[1].ptr, "robot info");
		char* send_pos      = strstr(Captures[1].ptr, "position");
		char* send_currents = strstr(Captures[1].ptr, "current");		
		char* send_speed    = strstr(Captures[1].ptr, "speed");
		char* send_frequency = strstr(Captures[1].ptr, "base frequency");
		if (send_status!=NULL)
		{	
			send_motor_status();	
			return TRUE;	
		} 
		if (send_robot!=NULL)
		{	
			send_robot_info();	
			return TRUE;	
		} 

		if (send_currents!=NULL)
		{	
			send_current_readings();	
			return TRUE;	
		} 
		if (send_pos!=NULL)
		{	
			send_positions();	
			return TRUE;	
		} 
		if (send_speed!=NULL)			
		{	
			send_speeds();		
			return TRUE;	
		} 
		if (send_frequency!=NULL)
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
	char buff[64];
	char expression[] = "home ([vV])? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?";
	int match = re_match( 6, Captures, expression, mIncoming );
	if (match) {
		sprintf(buff, "homing... ");
		for (int m=1; m<6; m++)
		{
			char temp[4];
			temp[0] = Captures[m].ptr[0];
			temp[1] = 0;
			//printf("Match %d: %d %.*s\n", m, Captures[m].len, Captures[m].len, Captures[m].ptr);
			strcat (buff, temp );			
			if ((Captures[m].ptr) && (Captures[m].len)) 
				set_motor_duty_letter( Captures[m].ptr[0], HOMING_SPEED );		
		}
		form_response(buff);
	}
	
	strcpy(expression, "stop");
	match = re_match( 6, Captures, expression, mIncoming );
	if (match) {
		stop_motors();
		return match;
	}	
	return match;
}

BOOL parse_duty_command( char* mIncoming )
{
	char buff[64];
	char expression[] = "^(duty|pwm) ?([vV]-?\\d+.\\d+)? ?([wW]-?\\d+.\\d+)? ?([xX]-?\\d+.\\d+)? ?([yY]-?\\d+.\\d+)? ?([zZ]-?\\d+.\\d+)?";
	int match = re_match( 7, Captures, expression, mIncoming );
	if (match) {
		sprintf(buff, "duty request ");
		//print_captures( 7, Captures);
		for (int m=2; m<=6; m++) 
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {

				const char* ptr   = Captures[m].ptr+1;
				float value = atof( ptr );
				
				set_motor_duty_letter( Captures[m].ptr[0], value );

				char temp[10];
				temp[0] = Captures[m].ptr[0];
				temp[1] = 0;
				strcat (buff, temp );
				
				dtostrf( value, 1, 3, temp );
				strcat (buff, temp );
				strcat (buff, " " );
			}
		}
		form_response(buff);
	}
	return match;
}

BOOL parse_position_command( char* mIncoming )
{
	char expression[] = "^position ([vV]\\d+.\\d+)? ?([wW]\\d+.\\d+)? ?([xX]\\d+.\\d+)? ?([yY]\\d+.\\d+)? ?([zZ]\\d+.\\d+)?";
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
	char expression[] = "^speed ([vV]\\d+)? ?([wW]\\d+.\\d+)? ?([xX]\\d+.\\d+)? ?([yY]\\d+.\\d+)? ?([zZ]\\d+.\\d+)?";
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
		BOOL is_enable = (strstr(Captures[1].ptr, "enable")==0);
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

BOOL parse_configuration( char* mIncoming )
{
	char expression[] = "save (configuration)?";
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {	
		save_configuration();
		return 1;
	}
	strcpy(expression, "read (configuration)?");
	match = re_match( 2, Captures, expression, mIncoming );
	if (match) {	
		read_configuration();		
		return 1;
	}
	return 0;
}

BOOL parse_firmware( char* mIncoming )
{
	char expression[] = "read (serial number|version)";
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {	
		BOOL is_sn       = (strstr(Captures[1].ptr, "serial number")!=NULL);
		BOOL is_revision = (strstr(Captures[1].ptr, "version")!=NULL);	
		char buff[100];		
		if (is_sn) {
			unsigned int tmp = (FiveMotorConfigData.serialNumber & 0xFFFF);
			sprintf(buff, "Serial#=%5d", tmp );	
			form_response(buff);
			return match;
		}
		if (is_revision)
			sprintf(buff,"Version=%d %d %d",
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
		set_base_PWM_freq( base_frequency );
		
		char temp[10];
		dtostrf( base_frequency, 3, 1, temp );		
		sprintf( buff, "Base frequency= %s\n", temp);
		form_response( buff );
	}
	return match;
}
/************************************************************************
 END MISCELLANEOUS COMMANDS.
*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************************************************************
 Differential Drive Train Commands 
	Specialized instructions for 2 wheeled robot movements.
*************************************************************************/
BOOL parse_diff_spin_command( char* mIncoming )
{
	char expression[] = "spin (-?\\d+.\\d?) degrees";
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {		
		float deg = atof(Captures[1].ptr);
		char buff[100];
		char temp[10];
		dtostrf( deg, 3, 1, temp );
		sprintf(buff, "Spinning %s degrees", temp );
		form_response(buff);
	}
	return match;
}

BOOL parse_diff_move_command( char* mIncoming )
{
	char expression[] = "(forward|backward) (-?\\d+.?\\d?)";
	int match = re_match( 3, Captures, expression, mIncoming );
	char buff[100];
	if (match) {		
		BOOL is_forward = (strstr(Captures[1].ptr, "forward")!=NULL);
		float value = atof( Captures[2].ptr );
		if (is_forward==FALSE) value = -value;
		set_motor_position( 'v', value );
		set_motor_position( 'w', value );
		char temp[10];
		dtostrf( value, 3, 1, temp );		
		sprintf(buff, "moving %s %s", temp, unit_to_string() );
		form_response(buff);
	}
	return match;
}

void send_motor_status()
{
	char buff[255];
	char temp[10];

	sprintf(buff, "Motor Status: %x %x %x %x %x", 
						FiveMotorConfigData.v_status, 
						FiveMotorConfigData.w_status,
						FiveMotorConfigData.x_status,
						FiveMotorConfigData.y_status,
						FiveMotorConfigData.z_status  );
	strcat(buff, "\r\n");	
	form_response(buff);
}

void send_robot_info()
{
	char buff[255];
	char temp[10];

	sprintf(buff, "Wheel diameter= " );	
	dtostrf( FiveMotorConfigData.wheel_diameter, 3, 1, temp );
	strcat(buff, temp );	
	strcat(buff, unit_to_string() );
	strcat(buff, "\r\n");
	
	strcat (buff, "Wheel separation= " );	
	dtostrf( FiveMotorConfigData.wheel_separation, 3, 1, temp );
	strcat(buff, temp );	
	strcat(buff, unit_to_string() );
	strcat(buff, "\r\n");
	
	strcat (buff, "Wheel circumference= " );	
	dtostrf( FiveMotorConfigData.wheel_circumference, 3, 1, temp );
	strcat(buff, temp );	
	strcat(buff, unit_to_string() );
	strcat(buff, "\r\n");
			
	form_response(buff);
}

BOOL parse_diff_settings( char* mIncoming )
{
	char buff[100];
	char expression[] = "set (wheel diameter|wheel separation) (\\d+.\\d?\\d?)";
	int match = re_match( 3, Captures, expression, mIncoming );
	if (match) {
		BOOL is_diameter = (strncmp(Captures[1].ptr, "wheel diameter", Captures[1].len )==0);
		if (is_diameter) {
			FiveMotorConfigData.wheel_diameter = atof( Captures[2].ptr );
			FiveMotorConfigData.wheel_circumference = M_PI * FiveMotorConfigData.wheel_diameter;			
			char temp[10];
			dtostrf( FiveMotorConfigData.wheel_diameter, 3, 1, temp );
			sprintf(buff, "wheel diameter=%s %s", temp, unit_to_string() );
			form_response(buff);
		} else {		
			FiveMotorConfigData.wheel_separation = atof( Captures[2].ptr );
			char temp[10];
			dtostrf( FiveMotorConfigData.wheel_separation, 3, 1, temp );
			sprintf(buff, "wheel separation=%s %s", temp, unit_to_string() );
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
	top_level( "pwm v100 x200 y300 z400" 		);
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
	top_level( "forward 50 "  	);
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
	
	//printf( "%s\n", mIncoming );
	
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
	retval = parse_configuration		( mIncoming );		if (retval) goto EXIT;
	
	retval = parse_set_base_frequency_command( mIncoming );	if (retval) goto EXIT;
	retval = parse_diff_spin_command		( mIncoming );	if (retval) goto EXIT;
	retval = parse_diff_move_command		( mIncoming );	if (retval) goto EXIT;
	retval = parse_diff_settings			( mIncoming );	if (retval) goto EXIT;
	
	if (strstr(mIncoming, "help")!=NULL)
	{	send_help();	goto EXIT;	};
	
		
	if (retval==FALSE)
		form_response( "NAK:unknown command." );	
EXIT:
	return;
	//if (ResponseReady) 
	//	printf( ">%s\n", Response );			
}

void send_help()
{
	char buffer[1024];
	sprintf(buffer,"KINETIC DRIVE FIVE PROTOCOL : \r\n\r\n");
	strcat(buffer,"use encoder          - positioning & speed are determined by the encoders;\r\n");	
	strcat(buffer,"use potentiometer    - positioning & speed are determined by the pots.;\r\n");	
	strcat(buffer,"Note: - pots normally do not support continuous rotation. Therefore,\r\n");
	strcat(buffer,"		more typically used in angular measurements.\r\n\r\n");	
	send_message( buffer );
		
	sprintf(buffer,"set unit counts|inches|feet|meter - select active unit;\r\n");
	strcat(buffer,"read unit            - display the currently selected unit;\r\n");	
	strcat(buffer,"read position        - read the position (based on pots or encoders);\r\n");	
	strcat(buffer,"read speed           - speed is deltas of the position;\r\n");
	strcat(buffer,"read base frequency  - the frequency of the pwm drive signals;\r\n");			
	strcat(buffer,"zero encoder         - mark current positions as 0 counts.\r\n");	
	strcat(buffer,"home                 - move all motors until near limit switch is triggered.\r\n");
	strcat(buffer,"measure travel       - measures the counts until far limit switch is triggered.\r\n\r\n");	
	send_message( buffer );

	sprintf(buffer,"pwm                 - set the duty cycle of each axis (values should range from 0.00 to 1.00\r\n");
	strcat(buffer,"enable limits        - limit switches will stop motor action.\r\n");	
	strcat(buffer,"disable limits       - limit switches are ignored.\r\n");					
	strcat(buffer,"read serial number   - for board identification.\r\n");
	strcat(buffer,"read version         - of this firmware.\r\n\r\n");	
	send_message( buffer );

	sprintf(buffer, "Robot 2 wheeled commands: v and w axes can be designated for a 2 wheel differential robot.\r\n\r\n");
	strcat(buffer,"set wheel separation - the distance (in selected unit) between the left & right wheels.\r\n");		
	strcat(buffer,"set wheel diameter   - the diameter (in selected unit) of the wheels.\r\n");		
	strcat(buffer,"spin __ degrees      - this instructs it to spin in place ending up facing a new direction.\r\n");		
	strcat(buffer,"forward __           - move robot forward __ number of units (whatever is selected)\r\n");		
	strcat(buffer,"backward __          - move robot forward __ number of units (whatever is selected)\r\n\r\n");
	send_message( buffer );

	send_prompt();
}

/* Five protocol

use encoder
use potentiometer
set unit - (counts|inch|cm|feet)
read unit - (counts|inch|cm|feet)

The following commands can specifify which AXIS to apply to : 
set|read %d counts_per_unit [vwxyz]
read position
read speed
read base frequency
zero encoder [vwxyz]
measure travel [vwxyz]
home [vwxyz]
pwm  v0.5 w0.75 x0.50 y0.50 z0.95

position v500 w650 x350 y400 z450
speed v20.0 w15.0 x10.0 y10.0 z10.0

enable limits [vwxyz]
disable limits [vwxyz]
read serial number
read version
set base frequency 500
spin 180 degrees
forward 100
backward 100
set wheel diameter 10
set wheel separation 50


*/
