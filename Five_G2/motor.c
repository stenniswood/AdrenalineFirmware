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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
#include "pid_controller.h"
#include "serial.h"


extern char buff[256];
const char motor_letters[NUM_MOTORS] = { 'v','w','x','y','z' };

struct stMotorState mot_states[NUM_MOTORS];
extern BOOL motors_stopped;		// see five_main.c


byte get_letter_index( char mMotorLetter )
{
	for (int i=0; i<NUM_MOTORS; i++)	
		if (motor_letters[i]==mMotorLetter)
			return i;
	return -1;		
}

void init_motor()
{
	DIRECTION_LINES_DDR  = DIRECTION_LINES_MASK;	// all outputs!
	DIRECTION_LINES_PORT = 0x55;	// 1 side high, 1 side low for each full hbridge	

	DIRECTION_LINES_G2_DDR  |= DIRECTION_LINES_G2_MASK;	// all outputs!
	DIRECTION_LINES_G2_PORT |= 0x10;	// 1 side high, 1 side low for each full hbridge	

	for (int i=0; i<NUM_MOTORS; i++)	
	{
		mot_states[i].duty_fraction     = 0.0;
		mot_states[i].direction   = true;
		mot_states[i].destination = 100;
		mot_states[i].pre_less_than_destination=true;		// position prior to set destination
		mot_states[i].direction 	= true;
		mot_states[i].PID_control 		= false;
		mot_states[i].sum_error 		= 0.0;
		mot_states[i].end_reported 		= FALSE;
	}
}

/* a low signal with a minimum length of treset at the INH
   When its temperature has decreased at least the thermal hysteresis ∆T = 7 deg celsius
   Trip point = 175 deg celsius
*/
void reset_overtemp_v(  )
{
	// stop PWM's.
	// Lower INH
	// wait Treset (3 us)
	// resume PWM	
}

void set_dir_motor_z( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_G2_PORT |=  (1<<MOT_1_DIR_R);
		DIRECTION_LINES_G2_PORT &= ~(1<<MOT_1_DIR_L);
		mot_states[4].direction = true;		
	}
	else 
	{
		DIRECTION_LINES_G2_PORT &= ~(1<<MOT_1_DIR_R);
		DIRECTION_LINES_G2_PORT |=  (1<<MOT_1_DIR_L);
		mot_states[4].direction = false;
	}
}

void set_dir_motor_y( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_PORT |=  (1<<MOT_2_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_2_DIR_L);
		mot_states[3].direction = true;
	}
	else 
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_2_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_2_DIR_L);
		mot_states[3].direction = false;		
	}	
}

void set_dir_motor_x( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_PORT |=  (1<<MOT_3_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_3_DIR_L);
		mot_states[2].direction = true;		
	}
	else 
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_3_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_3_DIR_L);
		mot_states[2].direction = false;
	}	
}

void set_dir_motor_w( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_PORT |=  (1<<MOT_4_DIR_R);
		DIRECTION_LINES_PORT &= ~(1<<MOT_4_DIR_L);
		mot_states[1].direction = true;				
	}
	else 
	{
		DIRECTION_LINES_PORT &= ~(1<<MOT_4_DIR_R);
		DIRECTION_LINES_PORT |=  (1<<MOT_4_DIR_L);
		mot_states[1].direction = false;				
	}
}

void set_dir_motor_v( bool mForward )
{
	if (mForward)
	{
		DIRECTION_LINES_G2_PORT |=  (1<<MOT_5_DIR_L);
		DIRECTION_LINES_G2_PORT &= ~(1<<MOT_5_DIR_R);
		mot_states[0].direction = true;
	}
	else 
	{
		DIRECTION_LINES_G2_PORT &= ~(1<<MOT_5_DIR_L);
		DIRECTION_LINES_G2_PORT |=  (1<<MOT_5_DIR_R);
		mot_states[0].direction = false;
	}
}

void set_dir_motor_letter( char Letter, bool mForward )
{
	switch( Letter )
	{
	case 'v': set_dir_motor_v(  mForward );	break;
	case 'w': set_dir_motor_w(  mForward );	break;
	case 'x': set_dir_motor_x(  mForward );	break;
	case 'y': set_dir_motor_y(  mForward );	break;
	case 'z': set_dir_motor_z(  mForward );	break;
	default : break;
	};
}

/**********************************************************/
/************ END OF DIRECTION CONTROL ********************/
/**********************************************************/

/* Don't use. */
void set_motor_w_brake( bool mBrake )
{
	if (mBrake)
	{
		DIRECTION_LINES_G2_PORT &= ~(1<<MOT_5_DIR_R);
		DIRECTION_LINES_G2_PORT &= ~(1<<MOT_5_DIR_L);
	}
	else // coast
	{
		DIRECTION_LINES_G2_PORT |= ~(1<<MOT_5_DIR_R);
		DIRECTION_LINES_G2_PORT |=  (1<<MOT_5_DIR_L);
	}	
}

/************ END OF BRAKE CONTROL ********************/


/**********************************************************
	MOTOR DUTY FUNCTIONS
		Mapping into mot_states[] array:
				v=0, w=1, x=2, y=3, z=4
		into OCR's:
				v=4A, w=4B, x=1A, y=1B, z=1C
 **********************************************************/
/* mDuty	- 	[0 to 1.00]  Fraction  */
void set_duty_z( float mDuty )		// z 
{
	if (mDuty<0)  mDuty = -mDuty;	// make it positive
	
	mot_states[4].duty_fraction = mDuty;
	
	uint16_t counts = ceil( (float)TOP * mDuty );
	OCR1C_Write( counts );
}
void set_duty_y( float mDuty )		// y
{
	if (mDuty<0) mDuty = -mDuty;
	mot_states[3].duty_fraction = mDuty;
	uint16_t counts = ceil( (float)TOP * mDuty );
	OCR1B_Write( counts );
}

void set_duty_x( float mDuty )		// x
{
	if (mDuty<0) mDuty = -mDuty;
	mot_states[2].duty_fraction = mDuty;	
	uint16_t counts = ceil( (float)TOP * mDuty );
	OCR1A_Write( counts );
}
void set_duty_w( float mDuty )		// w
{
	if (mDuty<0) mDuty = -mDuty;
	mot_states[1].duty_fraction = mDuty;	
	uint16_t counts = ceil( (float)TOP * mDuty );
	OCR4B_Write( counts );
}
void set_duty_v( float mDuty )		// v 
{
	if (mDuty<0) mDuty = -mDuty;
	mot_states[0].duty_fraction = mDuty;	
	uint16_t counts = ceil( (float)TOP * mDuty );
	OCR4A_Write( counts );
}
/**********************************************************/
/***************** END OF DUTY CONTROL ********************/
/**********************************************************/

/* Internal variable control only - no external hardware involvment */
void set_motor_position_v( uint32_t Position )
{
	mot_states[0].pre_less_than_destination = (Encoders[0].Count < Position);
	mot_states[0].direction = mot_states[0].pre_less_than_destination;	// positive direction
	mot_states[0].destination = Position;
}
void set_motor_position_w( uint32_t Position )
{
	mot_states[1].pre_less_than_destination = (Encoders[1].Count < Position);
	mot_states[1].direction = mot_states[1].pre_less_than_destination;	// positive direction
	mot_states[1].destination = Position;
}
void set_motor_position_x( uint32_t Position )
{
	mot_states[2].pre_less_than_destination = (Encoders[2].Count < Position);
	mot_states[2].direction = mot_states[2].pre_less_than_destination;	// positive direction
	mot_states[2].destination = Position;
}
void set_motor_position_y( uint32_t Position )
{
	mot_states[3].pre_less_than_destination = (Encoders[3].Count < Position);
	mot_states[3].direction = mot_states[3].pre_less_than_destination;	// positive direction
	mot_states[3].destination = Position;
}
void set_motor_position_z( uint32_t Position )
{
	mot_states[4].pre_less_than_destination = (Encoders[4].Count < Position);
	mot_states[4].direction = mot_states[4].pre_less_than_destination;	// positive direction
	mot_states[4].destination = Position;
}
void set_motor_position_letter( char Letter, uint32_t Position )
{
	switch( Letter )
	{
	case 'v': set_motor_position_v(  Position );	break;
	case 'w': set_motor_position_w(  Position );	break;
	case 'x': set_motor_position_x(  Position );	break;
	case 'y': set_motor_position_y(  Position );	break;
	case 'z': set_motor_position_z(  Position );	break;
	default : break;
	}
}

/************ END OF DESTINATION CONTROL ********************/

void stop_motors()
{
	// Don't Store in MotDuties[] for resume purposes.
	OCR1A_Write( 0 );
	OCR1B_Write( 0 );
	OCR1C_Write( 0 );
	OCR4A_Write( 0 );
	OCR4B_Write( 0 );
	motors_stopped = TRUE;
}

void resume_motors()
{
	set_duty_v( mot_states[0].duty_fraction );
	set_duty_w( mot_states[1].duty_fraction );
	set_duty_x( mot_states[2].duty_fraction );
	set_duty_y( mot_states[3].duty_fraction );
	set_duty_z( mot_states[4].duty_fraction );
}

/*void set_motor_duty( int mIndex, float mFraction )
{
	switch( mIndex)
	{
	case 0: set_duty_v(  mFraction );	break;
	case 1: set_duty_w(  mFraction );	break;
	case 2: set_duty_x(  mFraction );	break;
	case 3: set_duty_y(  mFraction );	break;
	case 4: set_duty_z(  mFraction );	break;
	default : break;
	}
}*/

void set_motor_duty_letter( char Letter, float mFraction )
{
	bool forward = (mFraction > 0.0);
	set_dir_motor_letter( Letter, forward );
	switch( Letter )
	{
	case 'v': set_duty_v(  mFraction );	break;
	case 'w': set_duty_w(  mFraction );	break;
	case 'x': set_duty_x(  mFraction );	break;
	case 'y': set_duty_y(  mFraction );	break;
	case 'z': set_duty_z(  mFraction );	break;
	default : break;
	};
}

void  set_motor_speed_letter( char Letter, float mFraction )  
{
	/* Fill this in later - pid control for a speed */
	/* For now, just set the duty! */
	switch( Letter )
	{
	case 'v': set_duty_v(  mFraction );	break;
	case 'w': set_duty_w(  mFraction );	break;
	case 'x': set_duty_x(  mFraction );	break;
	case 'y': set_duty_y(  mFraction );	break;
	case 'z': set_duty_z(  mFraction );	break;
	default : break;
	};	
}


void check_limit_disable( int mIndex )
{
	bool near = get_limit_switch_near( mIndex );
	bool far  = get_limit_switch_far ( mIndex );
	
	if ((near) && (mot_states[mIndex].direction))
	{
		set_motor_duty_letter( motor_letters[mIndex], 0.0);
	} 
	else if ((far) && (mot_states[mIndex].direction==0))
	{
		set_motor_duty_letter( motor_letters[mIndex], 0.0);
	}
}

void pid_control(int i)
{	
	float duty  = 0.0;
	char letter = motor_letters[i];

	duty = PI_algo( letter );
	set_motor_duty_letter( letter, duty );


	if ((end_point_reached(i)) && (mot_states[i].end_reported==FALSE))
	{
	    sprintf(buff, "%c PID done.", letter );
		send_message( (char*)buff );
		mot_states[i].end_reported = TRUE;
	}
	if (end_point_settled(i))
		mot_states[i].PID_control = FALSE;
}

/* This slowly changes the duty - to avoid sparks 0 to 100% or 100% to -100% */
void ramp_duty()
{
	// Ramp up
	for (int m=0; m<6; m++)
	{
		float delta = (mot_states[m].duty_fraction - mot_states[m].realized_duty);
		if (delta>0.0) {
			mot_states[m].realized_duty += 0.10;
			if (mot_states[m].realized_duty > mot_states[m].duty_fraction)
				mot_states[m].realized_duty = mot_states[m].duty_fraction;			
		}
		else {
			mot_states[m].realized_duty -= 0.10;
			if (mot_states[m].realized_duty < mot_states[m].duty_fraction)
				mot_states[m].realized_duty = mot_states[m].duty_fraction;		
		}
	}

	// Ramp Down
	for (int m=0; m<6; m++)
	{
		float delta = (mot_states[m].duty_fraction - mot_states[m].realized_duty);
		if (delta<0.0) {
			mot_states[m].realized_duty = mot_states[m].realized_duty * 0.90;
			if (mot_states[m].realized_duty < mot_states[m].duty_fraction)
				mot_states[m].realized_duty = mot_states[m].duty_fraction;			
		}
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
	check_limit_disable(0);
	check_limit_disable(1);
	check_limit_disable(2);
	check_limit_disable(3);
	check_limit_disable(4);
	
	if (motors_stopped==FALSE) 
		for (int i=0; i<NUM_MOTORS; i++)
		{
			if (mot_states[i].PID_control)
					pid_control(i);
		}
		
	//ramp_duty();		
}


char* get_status_string( byte mStatusByte )
{
	static char buff[40];
	
	if ((mStatusByte & 0xff)==0)	strcpy(buff, "okay");
	if (mStatusByte & 0x01)			strcpy(buff, "over temperature");
	if (mStatusByte & 0x02)			strcpy(buff, "over current");
	if (mStatusByte & 0x04)			strcpy(buff, "not reponding");	
	return buff;
}
