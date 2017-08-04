#ifndef _BUTTONS_H_
#define _BUTTONS_H_


#define PUSH_BUTTONS_DDR  DDRJ
#define PUSH_BUTTONS_PORT PORTJ
#define PUSH_BUTTONS_PIN  PINJ
#define PUSH_BUTTON_1 (1<<5)			// prog
#define PUSH_BUTTON_2 (1<<6)			// stop

extern char button_1_state;
extern char button_2_state;

#define prog_button_pressed (button_1_state==0)
#define stop_button_pressed (button_2_state==0)


void init_buttons();
void read_buttons();




#endif
