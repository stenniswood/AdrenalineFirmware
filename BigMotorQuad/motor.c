/*********************************************************************
Product of Beyond Kinetics, Inc
This file puts together various other files:  
	pot.c calibrations.c encoder.c can_msg.c 
-------------------------------------------------------------------
DATE 	:  10/20/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include <avr/io.h>
#include "inttypes.h"
#include "pin_definitions.h"
#include "motor.h"
#include "leds2.h"
#include "pwm.h"
#include "encoder.h"
#include "pot.h"
#include "encoder.h"
#include "adc.h"
#include "motor.h"
#include "pin_definitions.h"
#include "limit_switches.h"


#define NUM_MOTORS 4
bool motor_direction[NUM_MOTORS];

void init_motor_control_lines()
{
	DIRECTION_LINES_DDR  = 0xFF;	// all outputs!
	DIRECTION_LINES_PORT = 0x55;	// 1 side high, 1 side low for each full hbridge	
	
	motor_direction[0] = true;
	motor_direction[1] = true;
	motor_direction[2] = true;
	motor_direction[3] = true;
}

void set_dir_motor_1( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_PORT |=  (1<<MOT_1_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_1_DIR_L);
		motor_direction[0] = true;		
	}
	else 
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_1_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_1_DIR_L);
		motor_direction[0] = false;
	}
}

void set_dir_motor_2( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_PORT |=  (1<<MOT_2_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_2_DIR_L);
		motor_direction[1] = true;
	}
	else 
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_2_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_2_DIR_L);
		motor_direction[1] = false;		
	}	
}

void set_dir_motor_3( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_PORT |=  (1<<MOT_3_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_3_DIR_L);
		motor_direction[2] = true;		
	}
	else 
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_3_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_3_DIR_L);
		motor_direction[2] = false;
	}	
}

void set_dir_motor_4( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_PORT |=  (1<<MOT_4_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_4_DIR_L);
		motor_direction[3] = true;				
	}
	else 
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_4_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_4_DIR_L);
		motor_direction[3] = true;				
	}
}

void set_motor_4_brake( bool mBrake )
{
	if (mBrake)
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_4_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_4_DIR_L);
	}
	else // coast
	{
		DIRECTION_LINES_PORT |= ~(1<<MOT_4_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_4_DIR_L);
	}	
}


void check_limit_disable( int mIndex )
{
	bool near = get_limit_switch_near( mIndex );
	bool far  = get_limit_switch_far ( mIndex );
	
	if ((near) && (motor_direction[mIndex]))
	{
		set_motor_duty(mIndex, 0.0);
	} 
	else if ((far) && (motor_direction[mIndex]==0))
	{
		set_motor_duty(mIndex, 0.0);
	}
}

/************************************************************
motor_timeslice()		Call this every 10ms

Read Position			Also update Speed & Acceleration
update duty (if pid)	
compare to limit		
compare to destination	
send postion report		
turn off if shutdown	
*************************************************************/
void motor_timeslice_10ms()
{
	check_limit_disable(1);
	check_limit_disable(2);
	check_limit_disable(3);
	check_limit_disable(4);
	
}




