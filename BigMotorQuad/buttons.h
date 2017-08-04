#ifndef _BUTTONS_H_
#define _BUTTONS_H_


#define PUSH_BUTTONS_DDR  DDRE
#define PUSH_BUTTONS_PORT PORTE
#define PUSH_BUTTONS_PIN  PINE

#define PUSH_BUTTON_1 (1<<2)
#define PUSH_BUTTON_2 (1<<3)

extern char button_1_state;
extern char button_2_state;

#define stop_button_pressed   (button_1_state==0)
#define resume_button_pressed (button_2_state==0)

void init_buttons();
void read_buttons();




#endif
