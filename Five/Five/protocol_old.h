#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_


#define HOMING_SPEED 0.5
#include "global.h"

void re_test();


bool is_in_set(byte theSet, byte Bit);
byte which_motors();

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
void send_positions();
void send_speeds();
void send_base_frequency();


#endif
