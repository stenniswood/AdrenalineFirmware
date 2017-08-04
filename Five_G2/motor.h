#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "global.h"

#ifndef NUM_MOTORS 
#define NUM_MOTORS 5
#endif

extern const char   motor_letters[];

struct stMotorState
{
	float		realized_duty;				// this ramps up to duty_fraction.
	float 		duty_fraction;
	bool		direction;
	uint32_t	destination;
	bool		pre_less_than_destination;		// position prior to set destination
	bool		PID_control;					// Last command was a PID request!
	float		sum_error;
	float		prev_error;						// for PID derivative term
	bool		end_reported;					// when PID reaches goal.
};
extern struct stMotorState mot_states[NUM_MOTORS];



#define DIRECTION_LINES_DDR  DDRC
#define DIRECTION_LINES_PORT PORTC
#define DIRECTION_LINES_MASK 0x3F
#define MOT_2_DIR_L		0
#define MOT_2_DIR_R		1
#define MOT_3_DIR_L		2
#define MOT_3_DIR_R		3
#define MOT_4_DIR_L		4
#define MOT_4_DIR_R		5

#define DIRECTION_LINES_G2_DDR  DDRJ
#define DIRECTION_LINES_G2_PORT PORTJ
#define DIRECTION_LINES_G2_MASK 0x0F
#define MOT_1_DIR_L		0
#define MOT_1_DIR_R		1
#define MOT_5_DIR_L		2
#define MOT_5_DIR_R		3


byte get_letter_index( char mMotorLetter );		// converts 'v''w''x''y''z' to the index [0,1,2,3,4]


void init_motor();

void set_dir_motor_z	( bool mForward );
void set_dir_motor_y	( bool mForward );
void set_dir_motor_x	( bool mForward );
void set_dir_motor_w	( bool mForward );
void set_dir_motor_v	( bool mForward );
void set_dir_motor_letter( char Letter, bool mForward );

void set_duty_v			( float mDuty );
void set_duty_w			( float mDuty );
void set_duty_x			( float mDuty );
void set_duty_y			( float mDuty );
void set_duty_z			( float mDuty );
void set_motor_duty_letter( char Letter, float mFraction );

void stop_motors		( );
void resume_motors		( );

void set_motor_speed_letter( char Letter, float mFraction );

void set_motor_position_v( uint32_t Position );
void set_motor_position_w( uint32_t Position );
void set_motor_position_x( uint32_t Position );
void set_motor_position_y( uint32_t Position );
void set_motor_position_z( uint32_t Position );
void set_motor_position_letter( char Letter, uint32_t Position );

void motor_timeslice_10ms();

char* get_status_string( byte mStatusByte );

#endif
