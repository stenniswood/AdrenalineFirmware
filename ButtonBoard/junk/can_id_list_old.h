#ifndef CAN_ID_LIST_h
#define CAN_ID_LIST_h

/* FOLLOWING IS A LIST OF THE IDENTIFIERS (16 bit):	
	Block ID ASSUMED 0x00
	Instance ID determined at RunTime.
*/

#define ID_BOARD_PRESENCE_REQUEST		0x0100
#define ID_BOARD_PRESENCE_BROADCAST		0x0101
#define ID_BOARD_INFO					0x0102
#define ID_BOARD_SERIAL_NUMBER			0x0103

#define ID_RESERVE_INSTANCE_REQUEST		0x0203
#define ID_INSTANCE						0x0204

#define ID_BUTTON_PRESSED				0x0305
#define ID_BUTTON_ROSTER				0x0306

// BUTTON BOARD :
#define ID_BUTTON_PRESSED				0x0305
#define ID_BUTTON_ROSTER				0x0306

// BIGMOTOR  RECEIVING MESSAGE IDs:
#define ID_MARK_STOP_1					0x0400
#define ID_MARK_STOP_2					0x0401
#define ID_MOVE_TO_ANGLE				0x0402
#define ID_MOVE_SPEED					0x0403
// BIGMOTOR  TRANSMITTING MESSAGE IDs:
#define ID_CALIBRATED_STOP_1			0x0410		// Upon request FOR DEBUG.
#define ID_CALIBRATED_STOP_2			0x0411		// Upon request FOR DEBUG.
#define ID_CURRENT_ANGLE				0x0412
#define ID_CURRENT_VALUE				0x0413
#define ID_CURRENT_SPEED				0x0414
#define ID_CURRENT_ACCEL				0x0415
#define ID_MOTOR_STATUS					0x0416


/************************* BOARD MODEL IDENTIFICATION NUMBERS **********************/
/* The following are to be sent as data[0] in the ID_BOARD_PRESENCE_BROADCAST message
 * They identify the model of the board.		 */
const byte ADRENALINE_BUTTON		= 0x01;
const byte ADRENALINE_TILT  		= 0x02;
const byte ADRENALINE_ANALOG  		= 0x03;
const byte ADRENALINE_BIGMOTOR		= 0x04;
const byte ADRENALINE_QUADCOPTER	= 0x05;
const byte ADRENALINE_LCD			= 0x06;

const byte ADRENALINE_POWER			= 0x07;
const byte ADRENALINE_BLUETOOTH		= 0x08;
const byte ADRENALINE_SMARTBACKPLANE= 0x09;
const byte ADRENALINE_ARDUINO		= 0x0A;

const byte ADRENALINE_EDGE			= 0xF0;
/************************* END BOARD MODEL IDENTIFICATION NUMBERS ******************/


#endif

