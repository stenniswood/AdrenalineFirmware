#ifndef _CANI_MSG_H_
#define _CANI_MSG_H_

void can_file_message( sCAN* mMsg );

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

//const long SerialNumber      = 0x56789CDE;

/* This module receives CAN messages:
	0x0010 	PWMs (0-3)
	0x0011 	PWMs (4-7)
	0x0012 	PWMs (8-11)
	0x0013 	PWMs (12-15)
	0x0014 	PWMs (16-19)
	0x0015 	PWMs (20-23)	
	0x0016 	PWMs (24-27)
	0x0017 	PWMs (28-31)
	0x0018 	PWMs (32-35)
	0x0019 	PWMs (36-39)	
	0x0020 	PWMs (40-43)
	0x0021 	PWMs (44-47)
	0x0022 	PWMs (48)
*/

#define CAN_PWM_BASE_ID 0x0010

// Each PWM is 12 bits
// Each Slot will only hold 4 PWMs however: 
#define CAN_PWM_SLOT_1_ID	CAN_PWM_BASE_ID
#define CAN_PWM_SLOT_2_ID	CAN_PWM_BASE_ID+1
#define CAN_PWM_SLOT_3_ID	CAN_PWM_BASE_ID+2
#define CAN_PWM_SLOT_4_ID	CAN_PWM_BASE_ID+3
#define CAN_PWM_SLOT_5_ID	CAN_PWM_BASE_ID+4
#define CAN_PWM_SLOT_6_ID	CAN_PWM_BASE_ID+5
#define CAN_PWM_SLOT_7_ID	CAN_PWM_BASE_ID+6
#define CAN_PWM_SLOT_8_ID	CAN_PWM_BASE_ID+7
#define CAN_PWM_SLOT_9_ID	CAN_PWM_BASE_ID+8
#define CAN_PWM_SLOT_10_ID	CAN_PWM_BASE_ID+9
#define CAN_PWM_SLOT_11_ID	CAN_PWM_BASE_ID+10
#define CAN_PWM_SLOT_12_ID	CAN_PWM_BASE_ID+11
#define CAN_PWM_LAST_SLOT_ID	CAN_PWM_BASE_ID+11
// The actual array is in spi_8500.c (externed in spi_8500.h)

word get_12Bit( byte* mData );
byte is_PWM_Msg( sCAN* mMsg );

void can_prep_board_id_msg	( sCAN* mMsg 	);
byte can_parse_pwm_msg_id	( uint16_t 		);		// given the ID, get the starting index into the PWM array.
void can_parse_pwm_msg		( sCAN* mMsg 	);		// Extract the PWM word array; Updates PWM array
void can_parse_cor_msg		( sCAN* mMsg );
void can_init_test_msg		(				);


void can_parse_tilt_msg( sCAN* mMsg );

#endif
