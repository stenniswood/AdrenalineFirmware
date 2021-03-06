#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "protocol.h"
#include "encoder.h"
#include "configuration.h"
#include "adc.h"
#include "serial.h"
#include "pwm.h"
#include "motor.h"
#include "differential.h"

#include "limit_switches.h"
#include "leds2.h"
#include "bk_system_defs.h"
#include "msg_sends.h"
#include "help.h"
#include "pid_controller.h"


extern BOOL motors_stopped;		// see five_main.c

//#include "testing_exports.h"
/************************************************************************
	REGEX EXPRESSIONS :
 ************************************************************************/
const char REGEX_USE_ENCODER[] 		PROGMEM = "use (encoders?|potentiometers?|POTS|pots) ?([vV]|all)? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?";
const char REGEX_SET_UNIT[] 		PROGMEM = "^(set|select|read) unit ?(counts|inch|meter|feet)?";
const char REGEX_GEARING[] 			PROGMEM = "(set|read) (\\d+) counts_per_unit ([vV]|all)? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?";
const char REGEX_ZERO_ENCODERS[] 	PROGMEM = "(zero|reset) (encoders?) ?([vV]|all)? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?";
const char REGEX_READ_POSITION[] 	PROGMEM = "read (position|pos|destination|speed|current|status|robot info|base frequency)";
const char REGEX_MEASURE_TRAVEL[] 	PROGMEM = "measure travel ?([vV])? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?";
const char REGEX_HOME[] 			PROGMEM = "home ?([vV|all])? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?";
const char REGEX_STOP[] 			PROGMEM = "stop";
const char REGEX_PWM[] 				PROGMEM = "^(duty|pwm|PWM) ?([vV] ?[+-]?\\d+.\\d+)? ?([wW] ?[+-]?\\d+.\\d+)? ?([xX] ?[+-]?\\d+.\\d+)? ?([yY] ?[+-]?\\d+.\\d+)? ?([zZ] ?[+-]?\\d+.\\d+)?";
const char REGEX_POS[] 				PROGMEM = "^(position|pos|POS) ?([vV] ?[+-]?\\d+.?\\d*)? ?([wW] ?[+-]?\\d+.?\\d*)? ?([xX] ?[+-]?\\d+.?\\d*)? ?([yY] ?[+-]?\\d+.?\\d*)? ?([zZ] ?[+-]?\\d+.?\\d*)?";
const char REGEX_SET_DESTINATION[] 	PROGMEM = "set destination ([vV][+-]?\\d+.?\\d*)? ?([wW][+-]?\\d+.?\\d*)? ?([xX][+-]?\\d+.?\\d*)? ?([yY][+-]?\\d+.?\\d*)? ?([zZ][+-]?\\d+.?\\d*)?";
const char REGEX_SET_SPEED[] 		PROGMEM = "^speed ([vV][+-]?\\d+.?\\d*)? ?([wW][+-]?\\d+.?\\d*)? ?([xX][+-]?\\d+.?\\d*)? ?([yY][+-]?\\d+.?\\d*)? ?([zZ][+-]?\\d+.?\\d*)?";
const char REGEX_ENABLE_LIMITS[] 	PROGMEM = "(enable|disable) limits? ?([vV]|all)? ?([wW])? ?([xX])? ?([yY])? ?([zZ])?";
const char REGEX_READ_VERSION[] 	PROGMEM = "read (serial number|version)";

//const char REGEX_CONFIGURATION[] 	PROGMEM = "(save|read|default) configuration";
const char REGEX_SET_BASE_FREQUENCY[] PROGMEM = "set base frequency (\\d+.?\\d?)";
//"verbose"
const char REGEX_SPIN[] 			PROGMEM = "spin (-?\\d+.\\d?) (degrees)?";
const char REGEX_MOVE[] 			PROGMEM = "(move|forward|backward) ([+-]?\\d+.?\\d?)";
const char REGEX_CURVE[] 			PROGMEM = "curve (left|right) ([+-]?\\d+.\\d?) (degrees)?,? ([+-]?\\d+.\\d?) radius";
const char REGEX_TURN[] 			PROGMEM = "turn (left|right) ([+-]?\\d+.\\d?) (degrees)?";
const char REGEX_SET_WHEELS[] 		PROGMEM = "set (wheel diameter|wheel separation|counts_per_rev) (\\d+.\\d?\\d?)";
const char REGEX_SET_KP[] 			PROGMEM = "set Kp ([vV] ?[+-]?\\d+.\\d+)? ?([wW] ?[+-]?\\d+.\\d+)? ?([xX] ?[+-]?\\d+.\\d+)? ?([yY] ?[+-]?\\d+.\\d+)? ?([zZ] ?[+-]?\\d+.\\d+)?";
const char REGEX_SET_KI[] 			PROGMEM = "set Ki ([vV] ?[+-]?\\d+.\\d+)? ?([wW] ?[+-]?\\d+.\\d+)? ?([xX] ?[+-]?\\d+.\\d+)? ?([yY] ?[+-]?\\d+.\\d+)? ?([zZ] ?[+-]?\\d+.\\d+)?";
const char REGEX_SET_KD[] 			PROGMEM = "set Kd ([vV] ?[+-]?\\d+.\\d+)? ?([wW] ?[+-]?\\d+.\\d+)? ?([xX] ?[+-]?\\d+.\\d+)? ?([yY] ?[+-]?\\d+.\\d+)? ?([zZ] ?[+-]?\\d+.\\d+)?";
const char REGEX_STREAM[] 			PROGMEM = "stream (\\d*)?";

PGM_P const regex_exp_table[] 		PROGMEM = 
{
	REGEX_USE_ENCODER, 	REGEX_SET_UNIT, REGEX_GEARING, REGEX_ZERO_ENCODERS,		/* 0..3 */
	REGEX_READ_POSITION, REGEX_MEASURE_TRAVEL, REGEX_HOME, REGEX_STOP, 			/* 4..7 */
	REGEX_PWM, REGEX_SET_DESTINATION, REGEX_SET_SPEED, REGEX_ENABLE_LIMITS,		/* 8..11 */
	REGEX_READ_VERSION,				// 12
	REGEX_SET_BASE_FREQUENCY, REGEX_SPIN, REGEX_MOVE, REGEX_CURVE, 				/* 13..16 */
	REGEX_TURN, REGEX_SET_WHEELS, REGEX_SET_KP, REGEX_SET_KI, REGEX_SET_KD,		/* 17..21 */
	REGEX_POS, REGEX_STREAM														/* 22..23 */
};


/************************************************************************
	REGEX UTILITY FUNCTIONS :
 ************************************************************************/
char  Response[RESPONSE_SIZE];
BOOL  ResponseReady = 0;

char expression[256];
struct cap	Captures[25];
char buff[256];

BOOL verbose_mode = FALSE;
#define DEBUG 0

void init_captures(int nPossCaptures, struct cap* captures)
{
	for (int i=0; i<nPossCaptures; i++) {
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
			send_message( (char*)"\r\n");
		} else
		{
			sprintf(buff, "Match %d: %d %.*s\n", i, captures[i].len, captures[i].len, captures[i].ptr);
			send_message( buff );
			send_message( (char*)"\r\n");
		}
}

struct slre	slre;
int re_match( int nPossCaptures, struct cap* captures, char* mExpression, char* mString )
{
	int len    = strlen(mString);
	memset(slre.data, 0, 256);
	memset(slre.code, 0, 256);
	
	init_captures(nPossCaptures, captures);
	if (DEBUG || verbose_mode) 
	{
		send_message((char*)"\r\nexpression=");	
		send_message(mExpression);
	}
	
	if (!slre_compile(&slre, mExpression )) {
		if (DEBUG|| verbose_mode) 
		{
			send_message((char*)":Error-compiling regex: ");	
			send_message( (char*)slre.err_str);
		}
		return 0;
	}		
	if (DEBUG || verbose_mode) {
		send_message( (char*)"data=");
		send_message( (char*)slre.data );
		send_message( (char*)"\t");
		char tmp[20];
		sprintf(tmp,"datasize=%d ", slre.data_size );
		send_message( (char*)tmp);
	}
	if (slre_match(&slre, mString, len, captures)) {
		//send_message((char*)" Match!\r\n");	
		return 1;
	} 
	else {
		//send_message((char*)" No match!\r\n");
		return 0;
	}
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


/************************************************************************
	TRANSMISIONS:
 ************************************************************************/
void form_response(const char* mText)
{	
	strcpy(Response, mText );
	strcat(Response, "\r\n");
	ResponseReady = TRUE;
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
	retval = round( units * FiveMotorConfigData.counts_per_unit[index] );
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

/*BOOL is_in_set(byte theSet, byte Bit)
{
	return ( theSet & (1<<Bit) );
}*/

/************************************************************************
	COMMAND PARSING:
 ************************************************************************/
BOOL parse_use_encoder( char* mIncoming )
{
	strcpy_P(expression, (PGM_P)pgm_read_word( &regex_exp_table[0]) );
	int match = re_match( 7, Captures, expression, mIncoming );
	if (!match) 
		return match;

	BOOL use_encoder = (strstr(Captures[1].ptr, "encoder")!=NULL);
	sprintf(buff,"");
	char temp[32];	
	for (int m=2; m<7; m++)
	{		
		if ((Captures[m].ptr) && (Captures[m].len))
		{
			char letter = Captures[m].ptr[0];
			byte index = get_letter_index( letter );		
			if (use_encoder) {
					FiveMotorConfigData.use_encoder[index] = 1;
					sprintf(temp,"%c using encoder, ", letter );
					strcat(buff, temp);					
			} else {
					FiveMotorConfigData.use_encoder[index] = 0;
					sprintf(temp,"%c using potentiometer, ", letter );
					strcat(buff, temp);
			}
		}
	}
	form_response(buff);
	return match;
}


/* System Wide Unit Useage - ie all axis.
		Commands such as position will be interpreted in this unit.
		Commands such as speed will be interpreted in this unit per second.
		
		Possible to have different counts per unit however for different gearing 
*/
BOOL parse_set_unit( char* mIncoming )
{
	/* Notes: G21 = "use millimeters";  G20 = "set inch";  	*/
	strcpy_P(expression, (PGM_P)pgm_read_word(&regex_exp_table[1]) );
	int match = re_match( 3, Captures, expression, mIncoming );
	if (match) {
		float factor=0.0;
		
		if ((strstr(Captures[1].ptr, "set")!=NULL) || 
		    (strstr(Captures[1].ptr, "select")!=NULL))
		{
			if ( (strstr(Captures[2].ptr, "counts")!=NULL) )	{
//				FiveMotorConfigData.counts_per_unit = 1;		// we loose prev info?
				
				FiveMotorConfigData.units  = counts;   		// enum eMeasuringUnit		
				form_response( "unit=counts" );
			}
			if ( (strstr(Captures[2].ptr, "inch")!=NULL) )	{
				factor = convert_count_per_unit( FiveMotorConfigData.units, inch);
				for (int i=0; i<NUM_MOTORS; i++)
					FiveMotorConfigData.counts_per_unit[i] *= factor;
				FiveMotorConfigData.units  = inch;				
				form_response( "unit=inch" );
			}
			if ( (strstr(Captures[2].ptr, "meter")!=NULL) )	{
				factor = convert_count_per_unit( FiveMotorConfigData.units, meter);
				for (int i=0; i<NUM_MOTORS; i++)
					FiveMotorConfigData.counts_per_unit[i] *= factor;
				FiveMotorConfigData.units  = meter;   		// enum eMeasuringUnit		
				form_response( "unit=meter" );
			}
			if ( (strstr(Captures[2].ptr, "feet")!=NULL) )	 {
				factor = convert_count_per_unit( FiveMotorConfigData.units, feet);
				for (int i=0; i<NUM_MOTORS; i++)
					FiveMotorConfigData.counts_per_unit[i] *= factor;
				FiveMotorConfigData.units  = feet;
				form_response( "unit=feet" );
			}

			sprintf(buff, "Factor=");
			char temp[10];
			// Convert value to string & append to our response:
			dtostrf( factor, 3, 3, temp );
			strcat (buff, temp );
			send_message( buff );

		} else {	// Read unit
			char buff[100];
			sprintf(buff, "unit= %s", unit_to_string() );
			form_response( buff );
		}
	}
	return match;
}

/* counts_per_unit */
BOOL parse_gearing( char* mIncoming ) 
{
	char buffTmp[64];
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[2] ) );
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
				char letter = Captures[m].ptr[0];
				int index   = get_letter_index( letter );
				FiveMotorConfigData.counts_per_unit[index] = counts;
				sprintf(buff, "%c counts per %s = %d \r\n", motor_letters[index], unit_to_string(), counts );
				send_message(buff);
			}
		}
		strcpy(buff, "done" );
		form_response( buff );
	}
	else 
	{
	    // Read counts per unit : 
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
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[3]) );
	int match = re_match( 8, Captures, expression, mIncoming );
	if (match) 
	{
		bool params_specified = (Captures[3].ptr) && (Captures[3].len);
		bool all_specified    = (Captures[3].ptr) && (strstr(Captures[3].ptr, "all") != NULL);
		if ((params_specified==FALSE)  || (all_specified))
			// They don't have to put "all", just "zero encoders" will work.
			for (int j=0; j<5; j++)
				Encoders[j].Count=0;
		else 
			for (int m=3; m<8; m++)
			{		
				if ((Captures[m].ptr) && (Captures[m].len))
				{
					byte index = get_letter_index( Captures[m].ptr[0] );
					Encoders[index].Count=0;
				}
			}
		form_response( "zero positioned" );	
	}
	return match;
}

BOOL parse_read_position( char* mIncoming )
{
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[4]) );
	int  match = re_match( 2, Captures, expression, mIncoming );
	if (match) {
		char* send_status   = strstr(Captures[1].ptr, "status");
		char* send_robot    = strstr(Captures[1].ptr, "robot info");
		char* send_pos      = strstr(Captures[1].ptr, "position");
		char* send_currents = strstr(Captures[1].ptr, "current");		
		char* send_speed    = strstr(Captures[1].ptr,   "speed");
		char* send_destination= strstr(Captures[1].ptr, "destination");		
		char* send_frequency = strstr(Captures[1].ptr,  "base frequency");
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
		if (send_destination!=NULL)			
		{	
			send_destinations();		
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
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[5]) );
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
	/* GCode:  G28 */
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[6]) );
	int match = re_match( 6, Captures, expression, mIncoming );
	if (match) {
		sprintf(buff, "homing... ");
		BOOL is_all = (strstr(Captures[1].ptr, "all") != NULL);		

		char temp[4];		
		if (is_all)	{  sprintf(temp, "all"); strcat(buff, temp); };
		
		for (int m=1; m<6; m++)
		{
			if (!is_all) {				
				temp[0] = Captures[m].ptr[0];		// motor letter
				temp[1] = 0;
				strcat (buff, temp );
			}
			if (((Captures[m].ptr) && (Captures[m].len)) || (is_all))
				set_motor_duty_letter( Captures[m].ptr[0], HOMING_SPEED );		
		}
		form_response(buff);
		return match;
	}
	
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[7]) );
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
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[8]) );
	int match = re_match( 7, Captures, expression, mIncoming );
	if (match) {
		sprintf(buff, "pwm request ");
		for (int m=2; m<=6; m++) 
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {
				char letter = Captures[m].ptr[0];
				int  mindex = get_letter_index( letter );
				const char* ptr   = Captures[m].ptr+1;
				while (*ptr == ' ') ptr++;			// skip space
				float value = atof( ptr );

				// Direction done inside set motor duty letter! 				
				motors_stopped = FALSE;
				mot_states[mindex].PID_control = FALSE;		// no longer.
				set_motor_duty_letter( letter, value );
				
				// Append the motor letter to our response:
				char temp[10];
				temp[0] = letter;
				temp[1] = 0;
				strcat (buff, temp );
				// Convert value to string & append to our response:
				dtostrf( value, 3, 4, temp );
				strcat (buff, temp );
				strcat (buff, " " );
			}
		}
		form_response(buff);
	}
	return match;
}

/* Set destination command */
BOOL parse_destination_command( char* mIncoming )
{	
	char tmp[128];
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[9]) );
	int match = re_match( 6, Captures, expression, mIncoming );
	if (match) {
	
		for (int m=1; m<=6; m++)
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {
				const char* ptr   = Captures[m].ptr+1;
				float value = atof( ptr );			
				long int count = convert_units_to_counts( Captures[m].ptr[0], value );
				set_motor_position_letter( Captures[m].ptr[0], count );	
				
				char temp[10];
				dtostrf( value, 3, 3, temp );
				sprintf(tmp, "%c Motor stopping at %s %s == %ld counts.\r\n", Captures[m].ptr[0], temp, 
								unit_to_string(), count );
				strcat (buff, tmp);				
			}
		}
		form_response(buff);		
	}
	return match;
}

/* POS command - pid control */
BOOL parse_position_command( char* mIncoming )
{	
	char tmp[128];
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[22]) );
	int match = re_match( 7, Captures, expression, mIncoming );
	if (match) {
		motors_stopped = FALSE;
		strcpy(buff, "PID:\r\n");
		for (int m=2; m<=6; m++)
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {
				const char* ptr   = Captures[m].ptr+1;
				char letter = Captures[m].ptr[0];
				
				float value = atof( ptr );			
				long int count = convert_units_to_counts( letter, value );
				start_pid_control( letter, count );	

				char temp[10];
				dtostrf( value, 3, 3, temp );
				sprintf(tmp, "%c POS control to %s %s == %ld counts.\r\n", letter, temp, 
								unit_to_string(), count );
				strcat (buff, tmp);				
			}
		}
		form_response(buff);		
	}
	return match;
}


BOOL parse_speed_command( char* mIncoming )
{
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[10]) );
	int match = re_match( 6, Captures, expression, mIncoming );
	if (match) {
		motors_stopped = FALSE;
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
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[11]) );
	int match = re_match( 7, Captures, expression, mIncoming );
	if (match) {
	
		BOOL is_enable   = (strstr(Captures[1].ptr, "enable")!=NULL);
		BOOL is_all_axis = strstr( Captures[2].ptr, "all" ) != NULL;
		if (is_all_axis)
		{
			for (int i=0; i<5; i++)
				FiveMotorConfigData.use_limits[i]=is_enable;			
		}
		for (int m=2; m<=6; m++)
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {
				enable_limit( Captures[m].ptr[0],  is_enable);
			}
		}	
		if (is_enable)
			form_response("limits enabled");
		else 
			form_response("limits disabled");
	}
	return match;
}


BOOL parse_firmware( char* mIncoming )
{
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[12]) );
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {	
		BOOL is_sn       = (strstr(Captures[1].ptr, "serial number")!=NULL);
		BOOL is_revision = (strstr(Captures[1].ptr, "version")!=NULL);	
		if (is_sn) {
//			int tmp = (FiveMotorConfigData.serialNumber & 0xFFFF);
			sprintf(buff, "Serial#=%5ld", FiveMotorConfigData.serialNumber );	
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
		strcpy( buff, "Configuration saved!");
		form_response( buff );
		return 1;
	}
	strcpy(expression, "read configuration");
	match = re_match( 2, Captures, expression, mIncoming );
	if (match) {	
		read_configuration_and_set();		
		strcpy( buff, "Configuration loaded from NVM!");
		form_response( buff );
		return 1;
	}
	strcpy(expression, "default (configuration)?");
	match = re_match( 2, Captures, expression, mIncoming );
	if (match) {	
		init_configuration();	
		set_base_PWM_freq( FiveMotorConfigData.base_frequency );	
		strcpy( buff, "Configuration reset to defaults.");
		form_response( buff );
		return 1;
	}

	strcpy_P(expression,  (PGM_P)pgm_read_word(& regex_exp_table[19]) );
	match = re_match( 7, Captures, expression, mIncoming );
	if (match) {
		sprintf(buff, "PID parameter: Kp=");
		for (int m=1; m<=6; m++) 
		{
			if ((Captures[m].ptr) && (Captures[m].len))
			{
				char letter = Captures[m].ptr[0];
				int index = get_letter_index(letter);				
				const char* ptr   = Captures[m].ptr+1;
				while (*ptr == ' ') ptr++;			// skip space
				
				float value = atof( ptr );				
				FiveMotorConfigData.Kp[index] = value;

				// Append the motor letter to our response:
				char temp[10];
				temp[0] = letter;
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
	
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[20]) );
	match = re_match( 7, Captures, expression, mIncoming );
	if (match) {	
		sprintf(buff, "PID parameter: Ki=");
		for (int m=1; m<=6; m++) 
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {
				char letter = Captures[m].ptr[0];
				int index = get_letter_index(letter);
				const char* ptr   = Captures[m].ptr+1;
				while (*ptr == ' ') ptr++;			// skip space
				float value = atof( ptr );
				FiveMotorConfigData.Ki[index] = value;

				// Append the motor letter to our response:
				char temp[10];
				temp[0] = letter;
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
	
	strcpy_P(expression,  (PGM_P)pgm_read_word(& regex_exp_table[21]) );
	match = re_match( 7, Captures, expression, mIncoming );
	if (match) {	
		sprintf(buff, "PID parameter: Kd=");
		for (int m=1; m<=6; m++) 
		{
			if ((Captures[m].ptr) && (Captures[m].len)) {
				char letter = Captures[m].ptr[0];
				int index = get_letter_index(letter);
				const char* ptr   = Captures[m].ptr+1;
				while (*ptr == ' ') ptr++;			// skip space
				float value = atof( ptr );				
				FiveMotorConfigData.Kd[index] = value;

				// Append the motor letter to our response:
				char temp[10];
				temp[0] = letter;
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

	return 0;
}

extern BOOL stream_position;
extern int  stream_interval;

/************************************************************************
 MISCELLANEOUS COMMANDS : 
	Specialized instructions for 2 wheeled robot movements.
*************************************************************************/
BOOL parse_set_base_frequency_command( char* mIncoming )
{
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[13]) );
	int match = re_match( 2, Captures, expression, mIncoming );
	if (match) {				
		float base_frequency = atof(Captures[1].ptr);
		set_base_PWM_freq( base_frequency );
		// We also want to update all the PWM width.  The values need to be recomputed
		// based on the new base_frequency.  ie. 50% duty with different roll-over count.
		for (int i=0; i<5; i++)
			set_motor_duty_letter(motor_letters[i], mot_states[i].duty_fraction );
			
		FiveMotorConfigData.base_frequency = base_frequency;
		
		char temp[10];
		dtostrf( base_frequency, 3, 1, temp );		
		sprintf( buff, "Base frequency= %s\n", temp);
		form_response( buff );
		return match;
	}

	// "stream" :	
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[23]) );	
	//strcpy(expression, "stream" );
	match = re_match( 2, Captures, expression, mIncoming );
	if (match) {	
		stream_interval = atoi(Captures[1].ptr);			
		stream_position = !stream_position;		
		sprintf( buff, "Stream mode= %d\n", stream_position);
		form_response( buff );
	}

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
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[14]) );		// SPIN
	int match = re_match( 3, Captures, expression, mIncoming );
	if (match) {
		float deg = atof(Captures[1].ptr);
		spin_robot_degrees( deg );
		
		// RESPONSE:
		char temp[10];
		dtostrf( deg, 3, 1, temp );
		sprintf(buff, "Spinning %s degrees", temp );
		form_response(buff);
		return match;
	}

	strcpy_P (expression, (PGM_P)pgm_read_word(& regex_exp_table[15]) );	// MOVE FORWARD/BACKWARD
	match = re_match( 3, Captures, expression, mIncoming );	
	if (match) {
		BOOL is_forward = (strstr(Captures[1].ptr, "forward")!=NULL);
		//char letter = 'v';			// v & w should have the same number of counts_per_unit.
		if (Captures[2].ptr) 	// we got a number
		{	
			float value = atof( Captures[2].ptr );		// the distance 
			if (is_forward==FALSE) value = -value; 
			move_forward( value );

			// Form Response : 
			char temp[64];
			dtostrf( value, 3, 1, temp );
			long int count = convert_units_to_counts( 'v', value );
			sprintf(buff, "moving %s %s ==> %ld counts; ", temp, unit_to_string(), count );
			sprintf(temp, " [v,w] destination = [%ld,%ld] counts",
						mot_states[0].destination,
						mot_states[1].destination );
			strcat (buff, temp);
			form_response(buff);			
		} else 
			form_response((char*)"foward/backward - bad distance given");
		return match;
	}
	
	// Curve right turn left (assumed 90 degrees)
	strcpy_P(expression,  (PGM_P)pgm_read_word(& regex_exp_table[16]) );	// CURVE 
	match = re_match( 4, Captures, expression, mIncoming );
	if (match) 
	{
		BOOL is_left = (strstr(Captures[1].ptr, "left")!=NULL);
		if ((Captures[2].ptr==NULL) && (Captures[2].len==0))
			return 0;
		if ((Captures[4].ptr==NULL) && (Captures[4].len==0))
			return 0;

		float deg    = atof(Captures[2].ptr);
		float radius = atof(Captures[3].ptr);
		float avg_speed = (Encoders[0].Speed + Encoders[1].Speed) / 2.0;
		if (is_left)
			curve_left ( deg, radius, avg_speed );
		else 
			curve_right( deg, radius, avg_speed );
		
		// Form response:
		char temp[10],temp2[10];
		dtostrf( deg, 3, 1, temp     );
		dtostrf( radius, 3, 1, temp2 );
		sprintf(buff, "Curving %s degrees around %s radius", temp, temp2 );
		form_response(buff);
		return match;
	}

	// Turn right/left around small radius
	strcpy_P(expression,  (PGM_P)pgm_read_word(& regex_exp_table[17]) );	// CURVE 
	match = re_match( 4, Captures, expression, mIncoming );
	if (match) 
	{
		BOOL is_left = (strstr(Captures[1].ptr, "left")!=NULL);
		if ((Captures[2].ptr==NULL) && (Captures[2].len==0))
			return 0;
		if ((Captures[4].ptr==NULL) && (Captures[4].len==0))
			return 0;

		float deg    = atof(Captures[2].ptr);
		float avg_speed = (Encoders[0].Speed + Encoders[1].Speed) / 2.0;
		if (is_left)
			turn_left ( deg, avg_speed );
		else 
			turn_right( deg, avg_speed );
		
		// Form response:
		char temp[10];
		dtostrf( deg, 3, 1, temp );
		if (is_left)
			sprintf(buff, "Turning left %s degrees", temp );
		else 
			sprintf(buff, "Turning right %s degrees", temp );
		form_response(buff);
		return match;
	}
	
	return FALSE;
}

BOOL parse_diff_settings( char* mIncoming )
{
	char temp[10];
	char buffTmp[30];
	strcpy_P(expression, (PGM_P)pgm_read_word(& regex_exp_table[18]) );
	int match = re_match( 3, Captures, expression, mIncoming );
	if (match) {
		byte v_index = 0;
		byte w_index = 1;	
		BOOL is_cpr = (strstr(Captures[1].ptr, "counts per rev")!=NULL);
		if (is_cpr)
		{
			// Calculate counts_per_unit from the counts per rev.  per rev is a more commonly known value.
			float cpr = atof( Captures[2].ptr );
			FiveMotorConfigData.wheel_counts_per_rev = cpr;
			FiveMotorConfigData.counts_per_unit[v_index] = FiveMotorConfigData.wheel_circumference/cpr;
			// Wheels have to be the same size!
			FiveMotorConfigData.counts_per_unit[w_index] = FiveMotorConfigData.counts_per_unit[v_index];
			
			// Formulate the response:
			dtostrf( cpr, 3, 1, temp );
			sprintf(buffTmp, "counts per rev=%s ", temp );
			strcat(buff, buffTmp);
			dtostrf( FiveMotorConfigData.counts_per_unit[w_index], 3, 1, temp );
			sprintf(buffTmp, "%s counts per %s ", temp, unit_to_string() );			
			strcat(buff, buffTmp);
			form_response(buff);
		}		
		BOOL is_diameter = (strstr(Captures[1].ptr, "wheel diameter") != NULL);
		if (is_diameter) {
			if (Captures[2].ptr)
				FiveMotorConfigData.wheel_diameter = atof( Captures[2].ptr );
			FiveMotorConfigData.wheel_circumference = M_PI * FiveMotorConfigData.wheel_diameter;
			// Update Counts per unit : 
			// Assume the counts_per_rev remains unchanged - since same code wheel.
			FiveMotorConfigData.counts_per_unit[v_index] = FiveMotorConfigData.wheel_circumference/FiveMotorConfigData.wheel_counts_per_rev;
			FiveMotorConfigData.counts_per_unit[v_index] = FiveMotorConfigData.counts_per_unit[v_index];			
			
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



BOOL is_empty_command( char* mIncoming )
{
	int length = strlen(mIncoming);
	if (length==0) return TRUE;
	
	// Scan:  A space, tab or \n are still considered empty!
	for (int i=0; i<length; i++)
		if ((mIncoming[i]!=' ') && (mIncoming[i]!='\t') && (mIncoming[i]!='\n') ) 
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

	retval = is_empty_command			( mIncoming );		if (retval) { ResponseReady=TRUE; return; };	
//	send_message("not-empty\r\n");
	retval = parse_use_encoder			( mIncoming );		if (retval) return;
	retval = parse_set_unit				( mIncoming );		if (retval) return;
	retval = parse_gearing				( mIncoming );		if (retval) return;
	retval = parse_zero_encoders		( mIncoming );		if (retval) return;
	retval = parse_read_position		( mIncoming );		if (retval) return;
	retval = parse_measure_travel		( mIncoming );		if (retval) return;
	retval = parse_home_command			( mIncoming );		if (retval) return;

	retval = parse_duty_command			( mIncoming );		if (retval) return;
	retval = parse_position_command 	( mIncoming );		if (retval) return;
	//retval = parse_destination_command 	( mIncoming );		if (retval) return;
	retval = parse_speed_command		( mIncoming );		if (retval) return;

	retval = parse_limits_enable		( mIncoming );		if (retval) return;
	retval = parse_firmware				( mIncoming );		if (retval) return;
	retval = parse_configuration		( mIncoming );		if (retval) return;
	
	retval = parse_set_base_frequency_command( mIncoming );	if (retval) return;
	retval = parse_diff_move_command		( mIncoming );	if (retval) return;
	retval = parse_diff_settings			( mIncoming );	if (retval) return;

	if (strstr(mIncoming, "help")!=NULL)
	{	send_help();	return;	};
	if (strstr(mIncoming, "list")!=NULL)
	{	send_list();	return;	};

	
	if (retval==FALSE) {
		if (verbose_mode)  dump_msg_hex(mIncoming);
		form_response( "NAK:unknown command." );	
	}

	return;
}

void top_level_caller( char* mIncoming )
{
	top_level( mIncoming );
	
	if (ResponseReady) 
	{
		if (strlen(Response)) {
			send_message( (char*)"\r\n" );		
			send_message( Response );	
		}
		led_off( SERIAL_LED );
		ResponseReady = 0;
		send_prompt( );
	}
}


