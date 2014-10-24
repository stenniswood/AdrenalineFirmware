#ifndef _ANALOG_H_
#define _ANALOG_H_

extern uint8_t 	ActiveSignals[8];		// 2 per analog chip
extern uint16_t	Readings[64];			// Analog Readings 


void an_init();				// 
void an_read_actives();		// Read from the Maxim 11624 chips.
void an_apply_filters();	//

void delay( long int mdelay );

// See can_msg.h for preping the CAN message buffer with Analog readings


#endif
