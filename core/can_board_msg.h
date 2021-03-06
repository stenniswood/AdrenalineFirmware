#ifndef _CAN_BOARD_MSG_H_
#define _CAN_BOARD_MSG_H_


extern byte 	System_Send_Status;		// Use in OS_Dispatch() to send the requested message

// INCOMING:
BOOL can_board_msg_responder		( sCAN* mMsg );
void can_parse_led_request			( sCAN* mMsg );
void can_parse_board_presence_request( sCAN* mMsg );
void can_parse_configure_request	( sCAN* mMsg );

void set_configure_callback			( void (*mCallback)(byte mByteChanged) );

// OUTGOING:
void can_prep_board_presence_msg	( sCAN* mMsg );		// Send Once on startup
void can_prep_serial_number_msg		( sCAN* mMsg );		// Send upon request only
void can_prep_board_revision_msg	( sCAN* mMsg );		// Send once on startup & upon request
void can_prep_board_description_msg	( sCAN* mMsg );		// Send upon request
void can_prep_configure				( sCAN* mMsg );
void can_board_timeslice			( );

// SYSTEM OUTGOING:
void can_prep_board_presence_request( sCAN* mMsg, byte mType );
void can_prep_system_led_msg  		( sCAN* mMsg, byte mDestinationInstance, byte mMode, byte mPattern = 0x0A );

#define NO_SHUTDOWN_PENDING 0
#define SHUTDOWN_PENDING 	1
#define SHUTDOWN_COMPLETE 	2
extern byte ShutDownState;

// HARDWARE INFO:
#define HARDWARE_REVISION 		'A'
#define MANUFACTURER 	  		0x0001	// Beyond Kinetics is Manufacturer #1!


/********************************************************************/
// FIRMWARE (Major.Minor)  (2.1)			UPDATE HERE!! ***********
#define SOFTWARE_MAJOR			0x01					//***********
#define SOFTWARE_MINOR			0x0F					//***********
// Need a separate CORE versus APP revision numbers!
// But if only 1 then this needs to go into the APP!
/********************************************************************/



/********************************************************************************/
// BOARD STATUS:
#define BS_ALL_OKAY				 	0x00
#define BS_AUX_DEVICE_NOT_CONNECTED 0x10		// Such as motor or pot
#define BS_ACTIVE_DTC				0x01		// Active Fault Codes
#define BS_CRITICAL_ACTIVE_DTC		0x02		// Critical Fault Codes Active
#define BS_HARDWARE_FAILURE		 	0x03		// Permanent Failure
#define BS_INTERMITTENT_CAN			0x05		// Such as motor or pot
#define BS_RECENT_WDT_RESET			0x06		// Watchdog timer expired recently
// etc.


#endif 
