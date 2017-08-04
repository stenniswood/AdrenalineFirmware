#ifndef _MOTOR_H_
#define _MOTOR_H_


#ifndef NUM_MOTORS 
#define NUM_MOTORS 5
#endif

struct stMotorState
{
	float 		duty_fraction;
	bool		motor_direction;
	uint32_t	motor_destination;
	bool		motor_destination_pre;		// position prior to set destination
};
extern struct stMotorState mot_states[NUM_MOTORS];


#define DIRECTION_LINES_DDR  DDRK
#define DIRECTION_LINES_PORT PORTK
#define DIRECTION_LINES_MASK 0xFF
#define MOT_2_DIR_L		0
#define MOT_2_DIR_R		1
#define MOT_3_DIR_L		2
#define MOT_3_DIR_R		3
#define MOT_4_DIR_L		4
#define MOT_4_DIR_R		5
#define MOT_5_DIR_L		6
#define MOT_5_DIR_R		7

#define DIRECTION_LINES_G2_DDR  DDRJ
#define DIRECTION_LINES_G2_PORT PORTJ
#define DIRECTION_LINES_G2_MASK 0x18
#define MOT_1_DIR_L		3
#define MOT_1_DIR_R		4


void init_motor();

void set_dir_motor_1	( bool mForward );
void set_dir_motor_2	( bool mForward );
void set_dir_motor_3	( bool mForward );
void set_dir_motor_4	( bool mForward );
void set_dir_motor_5	( bool mForward );

void set_duty_M1		( float mDuty );
void set_duty_M2		( float mDuty );
void set_duty_M3		( float mDuty );
void set_duty_M4		( float mDuty );
void set_duty_M5		( float mDuty );
void set_motor_duty		( int mIndex, float mFraction );
void set_motor_duty_letter( char Letter, float mFraction );

void stop_motors		( );
void resume_motors		( );
void set_motor_duty		( int mIndex, float mFraction );

void  set_motor_speed_letter( char Letter, float mFraction );


void set_motor_position_1( uint32_t Position );
void set_motor_position_2( uint32_t Position );
void set_motor_position_3( uint32_t Position );
void set_motor_position_4( uint32_t Position );
void set_motor_position_5( uint32_t Position );
void set_motor_position  ( int mIndex, uint32_t Position );
void set_motor_position_letter( char Letter, uint32_t Position );

void motor_timeslice_10ms();


#endif
