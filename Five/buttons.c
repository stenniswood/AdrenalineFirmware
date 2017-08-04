#include <avr/sfr_defs.h>
#include <avr/common.h>
#include <avr/io.h>
#include "buttons.h"



char button_1_state=1;
char button_2_state=1;


void init_buttons()
{
	PUSH_BUTTONS_DDR  &= ~PUSH_BUTTON_1;
	PUSH_BUTTONS_DDR  &= ~PUSH_BUTTON_2;
	
	// Use pull-ups:
	PUSH_BUTTONS_PORT |= PUSH_BUTTON_1;
	PUSH_BUTTONS_PORT |= PUSH_BUTTON_2;
}

void read_buttons()
{
	button_1_state = (PUSH_BUTTONS_PIN & PUSH_BUTTON_1);
	button_2_state = (PUSH_BUTTONS_PIN & PUSH_BUTTON_2);
}

