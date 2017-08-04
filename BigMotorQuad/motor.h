#ifndef _MOTOR_H_
#define _MOTOR_H_


#define DIRECTION_LINES_DDR  DDRC
#define DIRECTION_LINES_PORT PORTC
#define MOT_1_DIR_R		0
#define MOT_1_DIR_L		1
#define MOT_2_DIR_R		2
#define MOT_2_DIR_L		3
#define MOT_3_DIR_R		4
#define MOT_3_DIR_L		5
#define MOT_4_DIR_R		6
#define MOT_4_DIR_L		7


void init_motor_control_lines();


void set_dir_motor_1( bool mForward );
void set_dir_motor_2( bool mForward );
void set_dir_motor_3( bool mForward );
void set_dir_motor_4( bool mForward );

void motor_timeslice_10ms();


#endif
