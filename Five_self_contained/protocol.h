#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_


//#include "testing_exports.h"
#include "slre.h"
#include "global.h"

#define HOMING_SPEED 0.5

extern char  Response[1024];
extern BOOL  ResponseReady;


void init_captures  (int nPossCaptures, struct cap* captures);
void print_captures (int nPossCaptures, struct cap* captures);
int  re_match		(int nPossCaptures, struct cap* captures, char* mExpression, char* mString );

void re_test();
void test();


bool is_in_set(byte theSet, byte Bit);


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


void top_level( char* mIncoming );
void send_motor_status();
void send_robot_info();
void send_current_readings();
void send_positions();
void send_speeds();
void send_base_frequency();
void send_help();


#endif
