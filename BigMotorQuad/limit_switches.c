#include <avr/sfr_defs.h>
#include <avr/common.h>
#include <avr/io.h>
#include "limit_switches.h"



short limit_switch_state_A=0x00;
short limit_switch_state_B=0x00;



void init_limit_switches()
{
	// 0 is input:	
	LIMIT_SWITCHES_A_DDR  &= 0xF0;
	LIMIT_SWITCHES_B_DDR  &= 0x0F;
	
	// Port has to be a 1 for pull-ups:
	LIMIT_SWITCHES_A_PORT  &= 0xF0;
	LIMIT_SWITCHES_B_PORT  &= 0x0F;
	
	MCUCR &= PUD;  // default from reset anyway.
}

void read_limit_switches()
{
	limit_switch_state_A = (LIMIT_SWITCHES_A_PIN & LIMIT_SWITCHES_A_MASK);
	limit_switch_state_B = (LIMIT_SWITCHES_B_PIN & LIMIT_SWITCHES_B_MASK)>>4;
}

bool is_any_limit_triggered()
{
	return ((limit_switch_state_A != 0x0F) || (limit_switch_state_B != 0x0F));
}


bool get_limit_switch_near( int mIndex )
{
	switch (mIndex)
	{
	case 0 :	return (limit_switch_state_A & 0x01); 
				break;
	case 1 : 	return (limit_switch_state_A & 0x02);
				break;
	case 2 : 	return (limit_switch_state_A & 0x03); 
				break;
	case 3 : 	return (limit_switch_state_A & 0x04); 
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
	case 0 :	return (limit_switch_state_B & 0x01); 
				break;
	case 1 : 	return (limit_switch_state_B & 0x02); 
				break;
	case 2 : 	return (limit_switch_state_B & 0x03); 
				break;
	case 3 : 	return (limit_switch_state_B & 0x04);
				break;
	default : 
				break;	
	}	
	return false;	
}

