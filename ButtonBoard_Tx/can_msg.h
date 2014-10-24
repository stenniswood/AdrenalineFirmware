#ifndef _CANI_MSG_H_
#define _CANI_MSG_H_

// Each module sends a message to identify itself (type of device, and 
// SerialNumber)
const byte AdrenalineEdge_ID	= 0x01;
const byte AdrenalineTilt_ID  	= 0x02;
const byte AdrenalineAnalog_ID  = 0x03;
const byte AdrenalineButton_ID	= 0x04;
const byte AdrenalineBlue_ID	= 0x05;
const byte AdrenalineBigMotor_ID= 0x06;
const byte AdrenalineQuadCopter_ID= 0x06;
const byte AdrenalinePower_ID	= 0x07;

const long SerialNumber      = 0x56789CDE;



void can_prep_board_id_msg		( sCAN* mMsg 											  );
void can_prep_button_pressed_msg( sCAN* mMsg, char mKeyCharacter, byte mRow, byte mColumn );
void can_prep_button_roster_msg ( sCAN* mMsg, union uKeyRoster mkeypad );


#endif
