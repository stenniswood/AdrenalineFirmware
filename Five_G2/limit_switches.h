#ifndef _LIMIT_SWITCHES_H_
#define _LIMIT_SWITCHES_H_


/* Group A */
#define LIMIT_SWITCHES_A_DDR  DDRL
#define LIMIT_SWITCHES_A_PORT PORTL
#define LIMIT_SWITCHES_A_PIN  PINL
#define LIMIT_SWITCHES_A_MASK 0xFF

#define LIMIT_SWITCH_4  (1<<0)
#define LIMIT_SWITCH_3  (1<<1)
#define LIMIT_SWITCH_2  (1<<2)
#define LIMIT_SWITCH_1  (1<<3)
#define LIMIT_SWITCH_4B (1<<4)
#define LIMIT_SWITCH_3B (1<<5)
#define LIMIT_SWITCH_2B (1<<6)
#define LIMIT_SWITCH_1B (1<<7)


/* Group B */
#define LIMIT_SWITCHES_B_DDR  DDRH
#define LIMIT_SWITCHES_B_PORT PORTH
#define LIMIT_SWITCHES_B_PIN  PINH
#define LIMIT_SWITCHES_B_MASK 0x60
#define LIMIT_SWITCH_5  	  (1<<5)
#define LIMIT_SWITCH_5B 	  (1<<6)


void init_limit_switches();
void read_limit_switches();


// Used in Motor.c to disable:
bool get_limit_switch_near( int mIndex );
bool get_limit_switch_far ( int mIndex );
bool is_any_limit_triggered();

void limit_switch_timeslice();

void  enable_limit( char Letter, BOOL mEnable ) ;		// changes configuration.
void  send_limit_states();



#endif
