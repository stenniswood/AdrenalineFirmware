/********************************************************************
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
#include <math.h>
#include "inttypes.h"
#include "pin_definitions.h"
#include "motor.h"
#include "leds2.h"
#include "pwm.h"
#include "encoder.h"
#include "pot.h"
#include "adc.h"
#include "motor.h"
#include "pin_definitions.h"
#include "limit_switches.h"


char   motor_letters[NUM_MOTORS] = { 'v','w','x','y','z' };
struct stMotorState mot_states[NUM_MOTORS];


void init_motor()
{
	DIRECTION_LINES_DDR  = DIRECTION_LINES_MASK;	// all outputs!
	DIRECTION_LINES_PORT = 0x55;	// 1 side high, 1 side low for each full hbridge	

	DIRECTION_LINES_G2_DDR  |= DIRECTION_LINES_G2_MASK;	// all outputs!
	DIRECTION_LINES_G2_PORT |= 0x10;	// 1 side high, 1 side low for each full hbridge	

	for (int i=0; i<NUM_MOTORS; i++)	
	{
		mot_states[i].duty_fraction     = 0.0;
		mot_states[i].motor_direction   = true;
		mot_states[i].motor_destination = 100;
		mot_states[i].motor_destination_pre=true;		// position prior to set destination
		mot_states[i].motor_direction = true;
	}
}

void set_dir_motor_1( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_PORT |=  (1<<MOT_1_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_1_DIR_L);
		mot_states[0].motor_direction = true;		
	}
	else 
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_1_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_1_DIR_L);
		mot_states[0].motor_direction = false;
	}
}

void set_dir_motor_2( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_PORT |=  (1<<MOT_2_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_2_DIR_L);
		mot_states[1].motor_direction = true;
	}
	else 
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_2_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_2_DIR_L);
		mot_states[1].motor_direction = false;		
	}	
}

void set_dir_motor_3( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_PORT |=  (1<<MOT_3_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_3_DIR_L);
		mot_states[2].motor_direction = true;		
	}
	else 
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_3_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_3_DIR_L);
		mot_states[2].motor_direction = false;
	}	
}

void set_dir_motor_4( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_PORT |=  (1<<MOT_4_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_4_DIR_L);
		mot_states[3].motor_direction = true;				
	}
	else 
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_4_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_4_DIR_L);
		mot_states[3].motor_direction = true;				
	}
}

void set_dir_motor_5( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_G2_PORT |=  (1<<MOT_1_DIR_L);
		DIRECTION_LINES_G2_PORT &= ~(1<<MOT_1_DIR_R);
		mot_states[4].motor_direction = true;				
	}
	else 
	{
		DIRECTION_LINES_G2_PORT &= ~(1<<MOT_1_DIR_R);
		DIRECTION_LINES_G2_PORT |=  (1<<MOT_1_DIR_L);
		mot_states[4].motor_direction = true;				
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




/* value must be Positive! 
mDuty	- 	[0 to 1.00]  Fraction
*/
void set_duty_M1( float mDuty )
{
	if (mDuty<0)  mDuty = -mDuty;	// make it positive
	mot_states[0].duty_fraction = mDuty;
	uint16_t counts = ceil( (float)TOP * mDuty );
	OCR1C_Write( counts );
}

void set_duty_M2( float mDuty )
{
	if (mDuty<0) mDuty = -mDuty;
	mot_states[1].duty_fraction = mDuty;
	uint16_t counts = ceil( (float)TOP * mDuty );
	OCR1B_Write( counts );
}

void set_duty_M3( float mDuty )
{
	if (mDuty<0) mDuty = -mDuty;
	mot_states[2].duty_fraction = mDuty;	
	uint16_t counts = ceil( (float)TOP * mDuty );
	OCR1A_Write( counts );
}
void set_duty_M4( float mDuty )
{
	if (mDuty<0) mDuty = -mDuty;
	mot_states[3].duty_fraction = mDuty;	
	uint16_t counts = ceil( (float)TOP * mDuty );
	OCR4A_Write( counts );
}
void set_duty_M5( float mDuty )
{
	if (mDuty<0) mDuty = -mDuty;
	mot_states[4].duty_fraction = mDuty;	
	uint16_t counts = ceil( (float)TOP * mDuty );
	OCR4B_Write( counts );
}

void set_motor_position_1( uint32_t Position )
{
	mot_states[0].motor_destination_pre = (Encoders[0].Count < Position);
	mot_states[0].motor_destination = Position;
}
void set_motor_position_2( uint32_t Position )
{
	mot_states[1].motor_destination_pre = (Encoders[1].Count < Position);
	mot_states[1].motor_destination = Position;
}
void set_motor_position_3( uint32_t Position )
{
	mot_states[2].motor_destination_pre = (Encoders[2].Count < Position);
	mot_states[2].motor_destination = Position;
}
void set_motor_position_4( uint32_t Position )
{
	mot_states[3].motor_destination_pre = (Encoders[3].Count < Position);
	mot_states[3].motor_destination = Position;
}
void set_motor_position_5( uint32_t Position )
{
	mot_states[4].motor_destination_pre = (Encoders[4].Count < Position);
	mot_states[4].motor_destination = Position;
}
void set_motor_position( int mIndex, uint32_t Position )
{
	switch( mIndex)
	{
	case 0: set_motor_position_1(  Position );	break;
	case 1: set_motor_position_2(  Position );	break;
	case 2: set_motor_position_3(  Position );	break;
	case 3: set_motor_position_4(  Position );	break;
	case 4: set_motor_position_5(  Position );	break;
	default : break;
	}
}

void set_motor_position_letter( char Letter, uint32_t Position )
{
	switch( Letter )
	{
	case 'v': set_motor_position_1(  Position );	break;
	case 'w': set_motor_position_2(  Position );	break;
	case 'x': set_motor_position_3(  Position );	break;
	case 'y': set_motor_position_4(  Position );	break;
	case 'z': set_motor_position_5(  Position );	break;
	default : break;
	}
}


void stop_motors()
{
	// Don't Store in MotDuties[] for resume purposes.
	OCR1A_Write( 0 );
	OCR1B_Write( 0 );
	OCR1C_Write( 0 );
	OCR4A_Write( 0 );
	OCR4B_Write( 0 );
}

void resume_motors()
{
	set_duty_M1( mot_states[0].duty_fraction );
	set_duty_M2( mot_states[1].duty_fraction );
	set_duty_M3( mot_states[2].duty_fraction );
	set_duty_M4( mot_states[3].duty_fraction );
	set_duty_M5( mot_states[4].duty_fraction );
}

void set_motor_duty( int mIndex, float mFraction )
{
	switch( mIndex)
	{
	case 0: set_duty_M1(  mFraction );	break;
	case 1: set_duty_M2(  mFraction );	break;
	case 2: set_duty_M3(  mFraction );	break;
	case 3: set_duty_M4(  mFraction );	break;
	case 4: set_duty_M5(  mFraction );	break;
	default : break;
	}
}

void set_motor_duty_letter( char Letter, float mFraction )
{
	switch( Letter )
	{
	case 'v': set_duty_M1(  mFraction );	break;
	case 'w': set_duty_M2(  mFraction );	break;
	case 'x': set_duty_M3(  mFraction );	break;
	case 'y': set_duty_M4(  mFraction );	break;
	case 'z': set_duty_M5(  mFraction );	break;
	default : break;
	};
}

void  set_motor_speed_letter( char Letter, float mFraction )  
{
	/* Fill this in later - pid control for a speed */
	/* For now, just set the duty! */
	switch( Letter )
	{
	case 'v': set_duty_M1(  mFraction );	break;
	case 'w': set_duty_M2(  mFraction );	break;
	case 'x': set_duty_M3(  mFraction );	break;
	case 'y': set_duty_M4(  mFraction );	break;
	case 'z': set_duty_M5(  mFraction );	break;
	default : break;
	};	
}

void check_position_reached( int mIndex )
{	
	if (mot_states[mIndex].motor_destination_pre)		// approaching from 0.
	{
		// Stop when greater or equal than the destination (allow coast past)
		if ( Encoders[mIndex].Count >= mot_states[mIndex].motor_destination )
			set_motor_duty(mIndex, 0.0);
	}
	else 	// approaching from infinity
	{
		// Stop when less than or equal than the destination (allow coast past)
		if ( Encoders[mIndex].Count  <= mot_states[mIndex].motor_destination )
			set_motor_duty(mIndex, 0.0);	
	}
}

void check_limit_disable( int mIndex )
{
	bool near = get_limit_switch_near( mIndex );
	bool far  = get_limit_switch_far ( mIndex );
	
	if ((near) && (mot_states[mIndex].motor_direction))
	{
		set_motor_duty(mIndex, 0.0);
	} 
	else if ((far) && (mot_states[mIndex].motor_direction==0))
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
	check_limit_disable(5);	
	
/*	check_position_reached(1);
	check_position_reached(2);
	check_position_reached(3);
	check_position_reached(4);
	check_position_reached(5); */
}

