#include <avr/sfr_defs.h>
#include <avr/common.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "serial.h"

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
