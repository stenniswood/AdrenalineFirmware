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
#include "differential.h"

#include "testing_exports.h"


/************************************************************************
	REGEX UTILITY FUNCTIONS :
 ************************************************************************/
char  Response[RESPONSE_SIZE];
BOOL  ResponseReady = 0;

char expression[256];
struct cap	Captures[25];

char buff[1024];

BOOL verbose_mode = FALSE;
#define DEBUG 1

void init_captures(int nPossCaptures, struct cap* captures)
{
	for (int i=0; i<=nPossCaptures; i++) {
		captures[i].len=0;
		// delete first?
		captures[i].ptr=NULL;
	}
}

void print_captures(int nPossCaptures, struct cap* captures)
{
	//printf("Request length: %d %s\n", captures[0].len, captures[0].ptr);
	for (int i=1; i<=nPossCaptures; i++)
		if (captures[i].ptr==NULL)
		{
			sprintf(buff, "Match %d:nf\r\n", i);
			send_message( buff );
			send_message("\r\n");
		} else
		{
			sprintf(buff, "Match %d: %d %.*s\n", i, captures[i].len, captures[i].len, captures[i].ptr);
			send_message( buff );
			send_message("\r\n");
		}
}

struct slre	slre;
int re_match( int nPossCaptures, struct cap* captures, char* mExpression, char* mString )
{
	int len    = strlen(mString);
	memset(slre.data, 0, 256);
	memset(slre.code, 0, 256);
	
	init_captures(nPossCaptures, captures);
//	if (DEBUG || verbose_mode)
	{
		send_message((char*)"\r\nexpression=");	send_message(mExpression);
	}
	
	if (!slre_compile(&slre, mExpression )) {
//		if (DEBUG|| verbose_mode) 
		{
			send_message((char*)":Error-compiling regex: ");	
			send_message( (char*)slre.err_str);
		}
		send_message( (char*)"data=");
		send_message( (char*)slre.data );
		send_message( (char*)"\r\n");			
		char tmp[20];
		sprintf(tmp,"datasize=%d ", slre.data_size );
		send_message( (char*)tmp);

		return 0;
	}		
	send_message( (char*)" data=");
	send_message( (char*)slre.data );
	send_message( (char*)" ");			
	char tmp[20];
	sprintf(tmp," datasize=%d ", slre.data_size );
	send_message( (char*)tmp);

	send_message( (char*)" code=");
	for (int i=0; i<slre.code_size; i++)
	{
		printf("%x ", slre.code[i] );
	}
	printf("\n");
	printf("codesize=%d ", slre.code_size );


	if (slre_match(&slre, mString, len, captures)) {
		//send_message((char*)" Match!\r\n");	
		return 1;
	} 
	else {
		//send_message((char*)" No match!\r\n");
		return 0;
	}
//		if (DEBUG|| verbose_mode) 
	//send_message("\r\n");
}

char unit_label[20];
char* unit_to_string()
{
	char* retval = unit_label;
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

/************************************************************************
	TRANSMISIONS:
 ************************************************************************/
void form_response(const char* mText)
{	
	strcpy(Response, mText );
	strcat(Response, "\r\n");
	ResponseReady = TRUE;
}

void send_current_readings()
{
	char temp[10];
		
	sprintf(buff, "Current: ");
	for (int i=0; i<5; i++)
	{
		dtostrf( CurrentSampleAvg[i], 3, 1, temp );
		strcat( buff, temp );
		strcat( buff, " ");
	}	
	form_response( buff );
}

void send_positions()
{
	if (FiveMotorConfigData.use_encoder) {
		sprintf(buff,"Positions: v=%8ld, w=%8ld, x=%8ld, y=%8ld, z=%8ld", 
			Encoders[0].Count, Encoders[1].Count, Encoders[2].Count, Encoders[3].Count, Encoders[4].Count );
	} else {
		sprintf(buff,"Positions: v=%8d, w=%8d, x=%8d, y=%8d, z=%8d", 
			PotSample[0], PotSample[1], PotSample[2], PotSample[3], PotSample[4] );
	}
	form_response( buff );
}

void send_speeds()
{
	if (FiveMotorConfigData.use_encoder) {
		sprintf(buff,"Speeds (cps): v=%8ld, w=%8ld, x=%8ld, y=%8ld, z=%8ld", 
			Encoders[0].Speed, Encoders[1].Speed, Encoders[2].Speed, Encoders[3].Speed, Encoders[4].Speed );
	} else {
		sprintf(buff,"Speeds (cps): v=%8d, w=%8d, x=%8d, y=%8d, z=%8d", 
			PotSample[0], PotSample[1], PotSample[2], PotSample[3], PotSample[4] );	
	}
	form_response( buff );	
}

void send_base_frequency()
{
	float num = get_base_frequency_herz();
	char temp[10];
	dtostrf( num, 4, 1, temp );
	sprintf(buff,"Base Frequency (hz): %s", temp );
	form_response(buff);
}

void send_robot_info()
{
	char temp[40];

	sprintf(buff, "use_encoder = %d\r\n", (FiveMotorConfigData.use_encoder>0) );
	strcat(buff, "unit = " );	
	strcat(buff, unit_to_string() );
	strcat(buff, "\r\n");
	send_message( buff );

	sprintf(buff, "Base Frequency = " );	
	dtostrf( FiveMotorConfigData.base_frequency, 3, 1, temp );
	strcat(buff, temp );	
	strcat(buff, "\r\n");
	send_message( buff );

	sprintf(buff, "Wheel diameter= " );	
	dtostrf( FiveMotorConfigData.wheel_diameter, 3, 1, temp );
	strcat(buff, temp );	
	strcat(buff, unit_to_string() );
	strcat(buff, "\r\n");
	send_message( buff );
	
	sprintf(buff, "Wheel separation= " );	
	dtostrf( FiveMotorConfigData.wheel_separation, 3, 1, temp );
	strcat(buff, temp );	
	strcat(buff, unit_to_string() );
	strcat(buff, "\r\n");
	send_message( buff );
	
	sprintf(buff, "Wheel circumference= " );	
	// calc now b/c may have been stored under different unit.
	float circumf = FiveMotorConfigData.wheel_diameter * M_PI;
	dtostrf( circumf, 3, 1, temp );
	strcat(buff, temp );	
	strcat(buff, unit_to_string() );
	strcat(buff, "\r\n");
	send_message( buff );

	// List all unit to counts ratios:
	for (int m=0; m<5; m++)
	{
		sprintf(buff, "\t%c counts_per_unit=%ld %s\r\n", motor_letters[m], 
					FiveMotorConfigData.counts_per_unit[m], 
					unit_to_string() );
		send_message( buff );		
	}
	
	// List all motor status':
	sprintf(buff, "Motor Status':\r\n" );
	send_message( buff );	
	for (int m=0; m<5; m++)
	{
		sprintf(buff, "\t%c motor_status=%2x \r\n", motor_letters[m], 
					FiveMotorConfigData.motor_status[m]  );
		send_message( buff );		
	}
	
	// List Limit enable status:
	sprintf(buff, "Use Limits:\r\n" );
	send_message( buff );	
	for (int m=0; m<5; m++)
	{
		sprintf(buff, "\t%c limit enabled : %d \r\n", motor_letters[m], 
					FiveMotorConfigData.use_limits[m]  );
		send_message( buff );		
	}	

	// Uses the 'v' motor for the gearing.
	sprintf( buff, "Counts per rev= " );
	dtostrf( FiveMotorConfigData.wheel_counts_per_rev, 4, 1, temp );
	strcat ( buff,temp );
	strcat ( buff, "\r\n");	
	send_message( buff );
	//form_response(buff);
}


float motor_letter_to_index( char mMotorLetter )
{
	switch( mMotorLetter )
	{
	case 'v': return 0;	
	case 'w': return 1;	
	case 'x': return 2;	
	case 'y': return 3;	
	case 'z': return 4;	
	default : return 0; break;
	}
}

/* Must be in the currently select unit! */
long int convert_units_to_counts( char mMotorLetter, float units )
{
	long int retval = 0.0;
	int index = motor_letter_to_index( mMotorLetter );
	retval = units * FiveMotorConfigData.counts_per_unit[index];
	return retval;
}

/* Will be in the currently select unit! */
float convert_counts_to_units( char mMotorLetter, long int counts )
{
	float retval = 0.0;
	int index = motor_letter_to_index( mMotorLetter );
	retval = counts / FiveMotorConfigData.counts_per_unit[index];
	return retval;		
}

/************************************************************************
	COMMAND PARSING:
 ************************************************************************/
BOOL is_in_set(byte theSet, byte Bit)
{
	return ( theSet & (1<<Bit) );
}

BOOL parse_use_encoder( char* mIncoming )
{
	strcpy(expression, "use (encoders?|potentiometers?)" );
	int match = re_match( 2, Captures, expression, mIncoming );
	if (!match) 
		return match;

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


/* System Wide Unit Useage - ie all axis.
		Commands such as position will be interpreted in this unit.
		Commands such as speed will be interpreted in this unit per second.
		
		Possible to have different counts per unit however for different gearing 
*/
BOOL parse_set_unit( char* mIncoming )
{
	strcpy(expression, "^(set|select|read) unit ?(counts|inch|meter|feet)?" );
	int match = re_match( 3, Captures, expression, mIncoming );
	if (match) {
		if ((strstr(Captures[1].ptr, "set")!=NULL) || 
		    (strstr(Captures[1].ptr, "select")!=NULL))
		{
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
	char buffTmp[64];
	strcpy(expression, "(set|read) (\\d+) counts_per_unit ([vV]|all)? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?" );
	int  match = re_match( 2, Captures, expression, mIncoming );
	if (!match) return FALSE;

	/* Need to expand this for all axes! */
	if ( (strstr(Captures[1].ptr, "set")!=NULL) )  {
		int counts=0;
		if ((Captures[2].ptr) && (Captures[2].len))
			counts = atoi( Captures[2].ptr );
		for (int m=3; m<8; m++)
		{		
			if ((Captures[m].ptr) && (Captures[m].len)) {
				FiveMotorConfigData.counts_per_unit[m-3] = counts;
				sprintf(buffTmp, "%c %d counts per %s; ", motor_letters[m-3], counts, unit_to_string() );
				strcat (buff, buffTmp);
			}
		}
		form_response( buff );
	}
	else 
	{
	    // Read cpu
		for (int m=0; m<4; m++)
		{			
				int counts = FiveMotorConfigData.counts_per_unit[m];
				sprintf(buffTmp, "%c %d counts per %s; ", motor_letters[m], counts, unit_to_string() );
				strcat (buff, buffTmp);			
		}
		form_response( buff );
	}
	return match;
}

BOOL parse_zero_encoders( char* mIncoming )
{
	strcpy(expression, "(zero|reset) (encoders?) ?([vV]|all)? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?");
	int match = re_match( 8, Captures, expression, mIncoming );
	if (match) {
		if ((Captures[3].ptr) && (strstr(Captures[3].ptr, "all")!=NULL))
			for (int j=0; j<5; j++)
				Encoders[j].Count=0;
		else 
			for (int m=3; m<8; m++)
			{		
				if ((Captures[m].ptr) && (Captures[m].len))
				{
					Encoders[m-3].Count=0;
				}
			}
		form_response( "zero positioned" );	
	}
	return match;
}

BOOL parse_read_position( char* mIncoming )
{
	strcpy(expression, "read (position|speed|current|status|robot info|base frequency)" );
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


BOOL parse_measure_travel( char* mIncoming )
{
	strcpy(expression, "measure travel ?([vV])? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?" );
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
	strcpy(expression, "home ([vV])? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?" );
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
		sprintf(buff, "Stopping motors");
		form_response(buff);		
		return match;
	}	
	return match;
}

BOOL parse_duty_command( char* mIncoming )
{
	strcpy(expression, "^(duty|pwm) ?([vV] ?[+-]?\\d+.\\d+)? ?([wW] ?[+-]?\\d+.\\d+)? ?([xX] ?[+-]?\\d+.\\d+)? ?([yY] ?[+-]?\\d+.\\d+)? ?([zZ] ?[+-]?\\d+.\\d+)?" );
	int match = re_match( 7, Captures, expression, mIncoming );
	if (match) {
		sprintf(buff, "duty request ");
		//print_captures( 7, Captures);
		for (int m=2; m<=6; m++) 
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {

				const char* ptr   = Captures[m].ptr+1;
				if (*ptr == ' ') ptr++;			// skip space
				float value = atof( ptr );

				// Direction done inside set motor duty letter! 
				//bool forward = (mFraction > 0.0);
				//set_dir_motor_letter ( Letter, forward );				
				set_motor_duty_letter( Captures[m].ptr[0], value );

				// Append the motor letter to our response:
				char temp[10];
				temp[0] = Captures[m].ptr[0];
				temp[1] = 0;
				strcat (buff, temp );
				// Convert value to string & append to our response:
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
	strcpy(expression, "set destination ([vV]\\d+.\\d+)? ?([wW]\\d+.\\d+)? ?([xX]\\d+.\\d+)? ?([yY]\\d+.\\d+)? ?([zZ]\\d+.\\d+)?" );
	int match = re_match( 6, Captures, expression, mIncoming );
	if (match) {
		for (int m=1; m<=6; m++)
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {
				const char* ptr   = Captures[m].ptr+1;
				float value = atof( ptr );			
				// convert from unit to counts;
				float counts = convert_units_to_counts( Captures[m].ptr[0], value );
				int count = trunc(counts);
				set_motor_position_letter( Captures[m].ptr[0], count );	
				
				char temp[10];
				dtostrf( value, 1, 3, temp );
				sprintf(buff, "Motor stopping at %s %s == %d counts.", temp, unit_to_string(), count );
			}
		}
		form_response(buff);		
	}
	return match;
}

BOOL parse_speed_command( char* mIncoming )
{
	strcpy(expression, "^speed ([vV]\\d+)? ?([wW]\\d+.\\d+)? ?([xX]\\d+.\\d+)? ?([yY]\\d+.\\d+)? ?([zZ]\\d+.\\d+)?" );
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
	strcpy(expression, "(enable|disable) limits ?([wW]|all)? ?[vV]? ?[xX]? ?[yY]? ?[zZ]?" );
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

char READ_VERSION[] = "read (serial number|version)";

BOOL parse_firmware( char* mIncoming )
{
	strcpy(expression, READ_VERSION );
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {	
		BOOL is_sn       = (strstr(Captures[1].ptr, "serial number")!=NULL);
		BOOL is_revision = (strstr(Captures[1].ptr, "version")!=NULL);	
		if (is_sn) {
			int tmp = (FiveMotorConfigData.serialNumber & 0xFFFF);
			sprintf(buff, "Serial#=%5d", tmp );	
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

BOOL parse_configuration( char* mIncoming )
{
	strcpy(expression, "save (configuration)?" );
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
	strcpy(expression, "default (configuration)?");
	match = re_match( 2, Captures, expression, mIncoming );
	if (match) {	
		init_configuration();		
		return 1;
	}

/*	strcpy(expression, "set Kp (\\d+.\\d+)? ?([wW]|all)? ?[vV]? ?[xX]? ?[yY]? ?[zZ]?" );
	match = re_match( 7, Captures, expression, mIncoming );
	if (match) {	
		sprintf(buff, "PID Control: Kp=");
		for (int m=2; m<=6; m++) 
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {

				const char* ptr   = Captures[m].ptr+1;
				if (*ptr == ' ') ptr++;			// skip space
				float value = atof( ptr );
				FiveMotorConfigData.Kp[m-2] = value;

				// Append the motor letter to our response:
				char temp[10];
				temp[0] = Captures[m].ptr[0];
				temp[1] = 0;
				strcat (buff, temp );
				// Convert value to string & append to our response:
				dtostrf( value, 1, 3, temp );
				strcat (buff, temp );
				strcat (buff, " " );
			}
		}
		form_response(buff);
		return 1;
	}
	strcpy(expression, "set Ki (\\d+.\\d+)? ?([wW]|all)? ?[vV]? ?[xX]? ?[yY]? ?[zZ]?" );
	match = re_match( 7, Captures, expression, mIncoming );
	if (match) {	
		sprintf(buff, "PID Control: Kp=");
		for (int m=2; m<=6; m++) 
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {

				const char* ptr   = Captures[m].ptr+1;
				if (*ptr == ' ') ptr++;			// skip space
				float value = atof( ptr );
				FiveMotorConfigData.Ki[m-2] = value;

				// Append the motor letter to our response:
				char temp[10];
				temp[0] = Captures[m].ptr[0];
				temp[1] = 0;
				strcat (buff, temp );
				// Convert value to string & append to our response:
				dtostrf( value, 1, 3, temp );
				strcat (buff, temp );
				strcat (buff, " " );
			}
		}
		form_response(buff);
		return 1;
	}*/

	return 0;
}


/************************************************************************
 MISCELLANEOUS COMMANDS : 
	Specialized instructions for 2 wheeled robot movements.
*************************************************************************/
BOOL parse_set_base_frequency_command( char* mIncoming )
{
	strcpy(expression, "set base frequency (\\d+)" );
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {				
		float base_frequency = atof(Captures[1].ptr);
		set_base_PWM_freq( base_frequency );
		FiveMotorConfigData.base_frequency = base_frequency;
		
		char temp[10];
		dtostrf( base_frequency, 3, 1, temp );		
		sprintf( buff, "Base frequency= %s\n", temp);
		form_response( buff );
	}
//	return match;

	// For debug:	
	strcpy(expression, "verbose" );
	match = re_match( 1, Captures, expression, mIncoming );
	if (match) {				
		verbose_mode = !verbose_mode;		
		sprintf( buff, "verbose mode= %d\n", verbose_mode);
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
BOOL parse_diff_move_command( char* mIncoming )
{
	strcpy(expression, "spin (-?\\d+.\\d?) degrees" );	
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {		
		float deg = atof(Captures[1].ptr);
		char temp[10];
		dtostrf( deg, 3, 1, temp );
		sprintf(buff, "Spinning %s degrees", temp );
		form_response(buff);
	}
	return match;

	strcpy (expression, "(forward|backward) ([+-]?\\d+.?\\d?)");
	match = re_match( 3, Captures, expression, mIncoming );	
	if (match) {		
		BOOL is_forward = (strstr(Captures[1].ptr, "forward")!=NULL);
		char letter = 'v';			// v & w should have the same number of counts_per_unit.
		if (Captures[2].ptr) 	// we got a number
		{	
			float value = atof( Captures[2].ptr );
			if (is_forward==FALSE) value = -value;		
			float counts = convert_units_to_counts( letter, value );		
			uint32_t count = trunc(counts);			
			set_motor_position_letter( 'v', count );
			set_motor_position_letter( 'w', count );
			char temp[64];
			dtostrf( value, 3, 1, temp );		
			sprintf(buff, "moving %s %s == %ld counts", temp, unit_to_string(), count );
			sprintf(temp, "absolute destination == %ld counts", mot_states[1].motor_destination );
			strcat (buff, temp);
			form_response(buff);			
		} else 
			form_response((char*)"foward/backward - bad distance given");
	}
	return match;
	
	// turn right turn left (assumed 90 degrees)
	strcpy(expression, "curve (left|right) (-?\\d+.\\d?) degrees, (-?\\d+.\\d?) radius" );
	match = re_match( 4, Captures, expression, mIncoming );
	if (match) 
	{
		BOOL is_left = (strstr(Captures[1].ptr, "left")!=NULL);
		if ((Captures[2].ptr==NULL) && (Captures[2].len==0))
			return 0;
		if ((Captures[3].ptr==NULL) && (Captures[3].len==0))
			return 0;

		float deg    = atof(Captures[2].ptr);
		float radius = atof(Captures[3].ptr);
		float avg_speed = (Encoders[0].Speed + Encoders[1].Speed) / 2.0;
		if (is_left)
			curve_left ( deg, radius, avg_speed );
		else 
			curve_right( deg, radius, avg_speed );
		
		// Form response:
		char temp[10];
		dtostrf( deg, 3, 1, temp );
		sprintf(buff, "Spinning %s degrees", temp );
		form_response(buff);
	}
	return match;
}

BOOL parse_diff_settings( char* mIncoming )
{
	char temp[10];
	char buffTmp[30];
	strcpy(expression, "set (wheel diameter|wheel separation|counts per rev) (\\d+.\\d?\\d?)" );
	int match = re_match( 3, Captures, expression, mIncoming );
	if (match) {
		BOOL is_cpr = (strstr(Captures[1].ptr, "counts per rev")!=NULL);
		if (is_cpr)
		{
			// Calculate counts_per_unit from the counts per rev.  per rev is a more commonly known value.
			float cpr = atof( Captures[2].ptr );
			FiveMotorConfigData.counts_per_unit[0] = FiveMotorConfigData.wheel_circumference/cpr;
			FiveMotorConfigData.counts_per_unit[1] = FiveMotorConfigData.counts_per_unit[0];

			// Formulate the response:
			dtostrf( cpr, 3, 1, temp );
			sprintf(buffTmp, "counts per rev=%s ", temp );
			strcat(buff, buffTmp);
			dtostrf( FiveMotorConfigData.counts_per_unit[1], 3, 1, temp );
			sprintf(buffTmp, "%s counts per %s ", temp, unit_to_string() );			
			strcat(buff, buffTmp);
			form_response(buff);
		}		
		BOOL is_diameter = (strstr(Captures[1].ptr, "wheel diameter") != NULL);
		if (is_diameter) {
			if (Captures[2].ptr)
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

void send_motor_status()
{
	sprintf(buff, "Motor Status: %x %x %x %x %x", 
						FiveMotorConfigData.motor_status[0], 
						FiveMotorConfigData.motor_status[1],
						FiveMotorConfigData.motor_status[2],
						FiveMotorConfigData.motor_status[3],
						FiveMotorConfigData.motor_status[4]  );
	strcat(buff, "\r\n");	
	form_response(buff);
}




/*void test()
{
	printf   ("Testing Protocol...");
	
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
}*/

void dump_msg_hex( char* mIncoming )
{
	int length = strlen(mIncoming);
	send_message( (char*) "Incoming msg in hex: ");
	for (int i=0; i<length; i++)
	{
		sprintf(buff, "%2x ", mIncoming[i] );
		send_message( buff );
	}
}

BOOL is_empty_command( char* mIncoming )
{
	int length = strlen(mIncoming);
	if (length==0) return TRUE;
	
	for (int i=0; i<length; i++)
		if ((mIncoming[i]!=' ') && (mIncoming[i]!='\t')) 
			return FALSE;

	return TRUE;
}

/************************************************************************
 END Differential Drive Train Commands 
*************************************************************************/
void top_level( char* mIncoming )
{
	BOOL retval   = FALSE;
	Response[0]   = 0;
	ResponseReady = FALSE;

	retval = is_empty_command			( mIncoming );		if (retval) { send_prompt( ); return; };	
	printf("Not empty!\n");
	retval = parse_use_encoder			( mIncoming );		if (retval) return;
	printf("Not encoders!\n");
	retval = parse_set_unit				( mIncoming );		if (retval) return;
	retval = parse_gearing				( mIncoming );		if (retval) return;
	retval = parse_zero_encoders		( mIncoming );		if (retval) return;
	retval = parse_read_position		( mIncoming );		if (retval) return;
	retval = parse_measure_travel		( mIncoming );		if (retval) return;
	retval = parse_home_command			( mIncoming );		if (retval) return;

	retval = parse_duty_command			( mIncoming );		if (retval) return;
	retval = parse_position_command 	( mIncoming );		if (retval) return;
	retval = parse_speed_command		( mIncoming );		if (retval) return;

	retval = parse_limits_enable		( mIncoming );		if (retval) return;
	retval = parse_firmware				( mIncoming );		if (retval) return;
	retval = parse_configuration		( mIncoming );		if (retval) return;
	
	retval = parse_set_base_frequency_command( mIncoming );	if (retval) return;
	retval = parse_diff_move_command		( mIncoming );	if (retval) return;
	retval = parse_diff_settings			( mIncoming );	if (retval) return;

	if (strstr(mIncoming, "help")!=NULL)
	{	send_help();	return;	};
	
	if (retval==FALSE) {
		if (verbose_mode)  dump_msg_hex(mIncoming);
		form_response( "NAK:unknown command." );	
	}

	return;
	//if (ResponseReady) 
	//	printf( ">%s\n", Response );			
}

void top_level_caller( char* mIncoming )
{
	top_level( mIncoming );
	
	if (ResponseReady) 
	{
		//printf("Response %s\n", Response);
		send_message( Response );
		ResponseReady = 0;
		send_prompt( );
	}
}

void send_help()
{
	sprintf(buff,"KINETIC DRIVE FIVE PROTOCOL : \r\n\r\n");
	strcat(buff,"\tuse encoder          - positioning & speed are determined by the encoders;\r\n");	
	strcat(buff,"\tuse potentiometer    - positioning & speed are determined by the pots.;\r\n");	
	strcat(buff,"Note: - pots normally do not support continuous rotation. Therefore,\r\n");
	strcat(buff,"		more typically used in angular measurements.\r\n\r\n");	
	send_message( buff );
		
	sprintf(buff, "SETUP COMMANDS: \r\n");
	strcat(buff,"\tselect unit counts|inches|feet|meter - select active unit;\r\n");
	strcat(buff,"\tread unit            - display the currently selected unit;\r\n");	
	strcat(buff,"\tset [number] counts_per_unit (v,w,x,y,z) - number of encoder counts that make up the unit (ie 400 counts/inch)\r\n");	
	strcat(buff,"\tread counts_per_unit - displays counts per unit for all axes;\r\n");	
	strcat(buff,"\tenable limits (v,w,x,y,z,all)  - limit switches will stop motor action.\r\n");	
	strcat(buff,"\tdisable limits (v,w,x,y,z,all) - limit switches are ignored.\r\n");					
	send_message( buff );

	sprintf(buff, "FEEDBACK COMMANDS: \r\n");
	strcat(buff,"\tread position        - read the position (based on pots or encoders);\r\n");	
	strcat(buff,"\tread speed           - speed is deltas of the position;\r\n");
	strcat(buff,"\tread current         - the current through each motor;\r\n");	
	strcat(buff,"\tread status          - okay, over temp, over current, etc;\r\n");		
	strcat(buff,"\tread base frequency  - the frequency of the pwm drive signals;\r\n");			
	strcat(buff,"\tset base frequency [number] - the frequency of the pwm drive signals (Hz);\r\n");			
	strcat(buff,"\tzero encoder (v,w,x,y,z, all) - mark current positions as 0 counts.\r\n");	
	send_message( buff );

	sprintf(buff, "MOTOR MOVE COMMANDS: \r\n");
	strcat (buff,"\tpwm                  - set the duty cycle of each axis (values should range from 0.00 to 1.00\r\n");
	strcat (buff,"\tposition (v10500 w9999.5 x500 y450.5 z607 - Move the motors until position is reach (PID control).\r\n");	
	strcat (buff,"\tspeed (v300 w999.5 x200 y450 z607 - Move the motors at specified speed (PID control).\r\n");	
	strcat (buff,"\thome                 - move all motors until near limit switch is triggered.\r\n");
	strcat (buff,"\tmeasure travel (all,v,w,x,y,z) - measures the counts until far limit switch is triggered.\r\n");	
	strcat (buff,"\tstop                 - stop all motors.\r\n");
	send_message( buff );

	sprintf(buff, "2 WHEELED ROBOT COMMANDS:  v and w axes can be designated for a 2 wheel differential robot.\r\n");
	strcat(buff,"\tset wheel separation - the distance (in selected unit) between the left & right wheels.\r\n");		
	strcat(buff,"\tset wheel diameter   - the diameter (in selected unit) of the wheels.\r\n");		
	strcat(buff,"\tset counts per rev   - the number of counts per rev for convenience - this will update the counts_per_unit configuration value.\r\n");		
	strcat(buff,"\tspin [number] degrees - this instructs it to spin in place ending up facing a new direction.\r\n");		
	strcat(buff,"\tforward [number]     - move robot forward __ number of units (whatever is selected)\r\n");		
	strcat(buff,"\tbackward [number]    - move robot forward __ number of units (whatever is selected)\r\n");
	strcat(buff,"\tread robot info      - displays the calibrated info about the robot.\r\n\r\n");	
	send_message( buff );

	sprintf(buff, "CONFIGURATION:\r\n");
	strcat(buff,"\tsave configuration - Save all configurtion data to EEPROM.\r\n");		
	strcat(buff,"\tread configuration - Read all configurtion data from EEPROM..\r\n");		
	strcat(buff,"\tdefault configuration - Initialize configurtion data.\r\n");		
	strcat(buff,"\tread serial number   - for board identification.\r\n");
	strcat(buff,"\tread version         - of this firmware.\r\n\r\n");	
	send_message( buff );

	send_prompt();
}

/* Five protocol

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

/*BOOL parse_firmware_upgrade( char* mIncoming )
{
	char buff[100];
	char expression[] = "upgrade firmware ";
	int match = re_match( 1, Captures, expression, mIncoming );
	if (match) {
		// Jump to 0x7E00, 0x7C00, 0x7800, or 0x7000 depending on the fuse Bit
			for bootloader size (for atmega640) 
		
		(for mega2560):
			Boot reset address:  0x1F000, 0x1F800, 0x1FC00, 0x1FE00
			
		
	}
}*/
