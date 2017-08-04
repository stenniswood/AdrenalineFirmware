#include <avr/sfr_defs.h>
#include <avr/common.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "serial.h"
#include "motor.h"

#include "configuration.h"
#include "limit_switches.h"


short limit_switch_state_A=0x00;
short limit_switch_state_B=0x00;


void init_limit_switches()
{
	// 0 is input:	
	LIMIT_SWITCHES_A_DDR  &= ~LIMIT_SWITCHES_A_MASK;		// inputs=0
	LIMIT_SWITCHES_B_DDR  &= ~LIMIT_SWITCHES_B_MASK;		// 
	
	// Port has to be a 1 for pull-ups:
	LIMIT_SWITCHES_A_PORT  |= LIMIT_SWITCHES_A_MASK;
	LIMIT_SWITCHES_B_PORT  |= LIMIT_SWITCHES_B_MASK;
	
	//MCUCR &= PUD;  // default from reset anyway.
}

void read_limit_switches()
{
	limit_switch_state_A = (LIMIT_SWITCHES_A_PIN & LIMIT_SWITCHES_A_MASK);
	limit_switch_state_B = (LIMIT_SWITCHES_B_PIN & LIMIT_SWITCHES_B_MASK);
}

bool is_any_limit_triggered()
{	
	return ((limit_switch_state_A != LIMIT_SWITCHES_A_MASK) || 
			 (limit_switch_state_B != LIMIT_SWITCHES_B_MASK));
}


bool get_limit_switch_near( int mIndex )
{
	switch (mIndex)
	{
	case 0 :	return !(limit_switch_state_A & LIMIT_SWITCH_1); 
				break;
	case 1 : 	return !(limit_switch_state_A & LIMIT_SWITCH_2);
				break;
	case 2 : 	return !(limit_switch_state_A & LIMIT_SWITCH_3); 
				break;
	case 3 : 	return !(limit_switch_state_A & LIMIT_SWITCH_4); 
				break;
	case 4 : 	return !(limit_switch_state_B & LIMIT_SWITCH_5); 
				break;
	default :	
				break;	
	}	
	return false;
}

bool get_limit_switch_far( int mIndex )
{
	switch (mIndex)
	{
	case 0 :	return !(limit_switch_state_A & LIMIT_SWITCH_1B); 
				break;
	case 1 : 	return !(limit_switch_state_A & LIMIT_SWITCH_2B); 
				break;
	case 2 : 	return !(limit_switch_state_A & LIMIT_SWITCH_3B); 
				break;
	case 3 : 	return !(limit_switch_state_A & LIMIT_SWITCH_4B);
				break;
	case 4 : 	return !(limit_switch_state_B & LIMIT_SWITCH_5B);
				break;
	default : 
				break;	
	}	
	return false;	
}

void limit_switch_timeslice()
{
	read_limit_switches();
}

void  enable_limit( char Letter, BOOL mEnable ) 
{ 		
	switch (Letter)
	{
	case 'v' : FiveMotorConfigData.use_limits[0]=mEnable; 	break;
	case 'w' : FiveMotorConfigData.use_limits[1]=mEnable; 	break;
	case 'x' : FiveMotorConfigData.use_limits[2]=mEnable; 	break;
	case 'y' : FiveMotorConfigData.use_limits[3]=mEnable; 	break;
	case 'z' : FiveMotorConfigData.use_limits[4]=mEnable; 	break;
	default	 : break;
	}
}

void send_limit_states()
{
	char buff[60];
	bool far[5];
	bool near[5];	
	
	for (int i=0; i<5; i++)
		far[i] = get_limit_switch_far( i );

	for (int i=0; i<5; i++)
		near[i] = get_limit_switch_near( i );
	
	sprintf(buff, "Limit Switches State:\r\n");
	send_message( buff );
	
	for (int i=0; i<5; i++) {
		sprintf(buff, "%c - enabled=%d; near=%d;\tfar=%d\r\n", motor_letters[i], 
				FiveMotorConfigData.use_limits[i], near[i], far[i] );
		send_message( buff );
	}	
}

