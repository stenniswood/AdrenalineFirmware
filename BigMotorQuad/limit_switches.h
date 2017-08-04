#ifndef _LIMIT_SWITCHES_H_
#define _LIMIT_SWITCHES_H_


/* Group A */
#define LIMIT_SWITCHES_A_DDR  DDRG
#define LIMIT_SWITCHES_A_PORT PORTG
#define LIMIT_SWITCHES_A_PIN  PING
#define LIMIT_SWITCHES_A_MASK 0x0F

#define LIMIT_SWITCH_1 (1<<0)
#define LIMIT_SWITCH_2 (1<<1)
#define LIMIT_SWITCH_3 (1<<2)
#define LIMIT_SWITCH_4 (1<<3)


/* Group B */
#define LIMIT_SWITCHES_B_DDR  DDRF
#define LIMIT_SWITCHES_B_PORT PORTF
#define LIMIT_SWITCHES_B_PIN  PINF
#define LIMIT_SWITCHES_B_MASK 0xF0

#define LIMIT_SWITCH_1B (1<<4)
#define LIMIT_SWITCH_2B (1<<5)
#define LIMIT_SWITCH_3B (1<<6)
#define LIMIT_SWITCH_4B (1<<7)


void init_limit_switches();
void read_limit_switches();


// Used in Motor.c to disable:
bool get_limit_switch_near( int mIndex );
bool get_limit_switch_far ( int mIndex );
bool is_any_limit_triggered();


#endif
