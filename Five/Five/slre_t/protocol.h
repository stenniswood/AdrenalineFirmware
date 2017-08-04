#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_


//#include "testing_exports.h"
#include "slre.h"
#include "global.h"

#define HOMING_SPEED 0.5

#define RESPONSE_SIZE 512

extern char  Response[RESPONSE_SIZE];
extern BOOL  ResponseReady;


long int convert_units_to_counts( char mMotorLetter, float units  );
float    convert_counts_to_units( char mMotorLetter, long int counts );


void init_captures  (int nPossCaptures, struct cap* captures);
void print_captures (int nPossCaptures, struct cap* captures);
int  re_match		(int nPossCaptures, struct cap* captures, char* mExpression, char* mString );

void re_test();
void test();


BOOL is_in_set(byte theSet, byte Bit);

/* Inidividual Subordinate commands  */
BOOL parse_zero_encoders			 ( char* mIncoming );
BOOL parse_measure_travel			 ( char* mIncoming );
BOOL parse_encoder_string			 ( char* mIncoming );
BOOL parse_home_command				 ( char* mIncoming );
BOOL parse_speed_command			 ( char* mIncoming );
BOOL parse_position_command			 ( char* mIncoming );
BOOL parse_set_base_frequency_command( char* mIncoming );
BOOL parse_diff_spin_command		 ( char* mIncoming );
BOOL parse_diff_move_command		 ( char* mIncoming );


void top_level		 ( char* mIncoming );
void top_level_caller( char* mIncoming );		// prints the response afterward.

void send_motor_status();
void send_robot_info();
void send_current_readings();
void send_positions();
void send_speeds();
void send_base_frequency();
void send_help();


#endif
