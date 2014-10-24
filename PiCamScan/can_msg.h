#ifndef _CANI_MSG_H_
#define _CANI_MSG_H_


// FIRMWARE (Major.Minor)  (2.1)
#define MAJOR 0x01
#define MINOR 0x00
#define MANUFACTURER 0x0001				// Beyond Kinetics is Manufacturer #1 !

const long SerialNumber      = 0x56789CDE;


void can_file_message			( sCAN* mMsg 		);
void can_prep_button_pressed_msg( sCAN* mMsg 		);
void can_prep_button_roster_msg ( sCAN* mMsg, union uKeyRoster mkeypad	);
void can_init_test_msg			(	byte* tmp							);


#endif
