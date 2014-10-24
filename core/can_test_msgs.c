/* This file will setup messages for testing a receiver board. */

// Use to verify that multiple boards receive the messages simultaneously
// There was a defect observed when this was sending an 2 other boards were receiving.
// Top nibble was shown on 1 tilt board Leds and lower nibble on another.
// One started lagging the other.  Since this message is sent only once, it means
// 		Maybe a sign error somewhere top nibble is taken as signed char.
//		Or the receiver buffer wasn't getting serviced properly.
byte a = 0x11;
void send_test_msgs()
{
	a = 0x00;
	while (1) 
	{
		a += 0x11;
		msg1.data[0] = a;
		led_on (1);
		can_send_msg( 0, &msg1 	);
		delay  (one_second/2);
		led_off(1);
		delay  (one_second/2);
	} 
}
/************ EEPROM TESTS ************************************/
#define MAX_EEPROM 				512
#define EEPROM_DTC_BASEADDRESS	0	// pass to SaveCodes()

/**************************************************************/
void eeprom_test()
{
	// BLINKS THE VALUE STORED IN EEPROM UPON BOOT UP.
	
	// EEPROM : (AFTER TEST, PUT MOTOR_INIT() BACK IN!)
	PWM_LEFT_DDR  |= (1<<PWM_LEFT );
	PWM_RIGHT_DDR |= (1<<PWM_RIGHT);

	short addr = EEPROM_DTC_BASEADDRESS;
	uint8_t stored = eeprom_read_byte((unsigned char*)addr );
	eeprom_write_byte((unsigned char*)addr, 0xC3 );
	while (1) { 
		show_result_toggle( stored );
		delay(one_second);
	};
}

//////////////////////////////////////////////////////////////////////////////
////////////////////// TEST OUT GOING MESSAGES ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/***************************************************************
  can_prep_board_presence_request()  
  		Usually sent by the master (raspberry pi)
		This sends the request for board presence.  All modules 
		will respond!

INPUT:	mType - 
				0=> Board Presence
				1=> Board Revision
				2=> Board Serial Number
				3=> Board Description (text)
				
Note: 0x00 Instance for this msg => Intended for all receivers
***************************************************************/
void can_prep_board_presence_request( sCAN* mMsg, byte mType )
{
    mMsg->id 	  = create_CAN_eid( ID_BOARD_PRESENCE_REQUEST, 0x00 );
    mMsg->data[0] = mType;
    mMsg->header.DLC = 1;
    mMsg->header.rtr = 0;
}

/************************************************************
 can_prep_system_led_msg()
INPUT:	mDestinationInstance - The intended receiver (0=> all boards!)
		mMode - LED mode for the receiver.		 
 SYSTEM_LED_MODE_DEVICE			0x00	(no Pattern required)
 SYSTEM_LED_MODE_PATTERN		0x01	(lower 4 bits of Pattern form the leds)
 SYSTEM_LED_MODE_STROBE			0x02	(Pattern=0 => Strobe Off; Pattern=1 => Strobe on)
 SYSTEM_LED_MODE_MYINSTANCE		0x03	(no Pattern required - module knows it's own Instance)
 SYSTEM_LED_MODE_CONT_PATTERN 	0x04	(blinks Pattern continuously)

OUTPUT: mMsg will be filled in for sending.
************************************************************/
void can_prep_system_led_msg( sCAN* mMsg, byte mDestinationInstance, byte mMode, byte mPattern )
{
    mMsg->id 	  = create_CAN_eid( ID_SYSTEM_LED_REQUEST, mDestinationInstance );
    mMsg->data[0] = mMode;
    mMsg->data[1] = mPattern;		// Lower 4 bits are the pattern!
    mMsg->header.DLC = 2;
    mMsg->header.rtr = 0;
}

/************************************************************/
/*********** LCD TEST CODE - THE BIGMOTOR PACKAGES DATA UP TO TEST THE LCD **************/
// FOR DEBUG, SEND POT VALUE: 
#define LCD_CMD_NO_ACTION			0x00
#define LCD_CMD_CURSOR_TO			0x01		// Change location where the next DrawText will go.
#define LCD_CMD_SELECT_FONT			0x02		// Change location where the next DrawText will go.
#define LCD_CMD_CLEAR_SCREEN		0x03		// Clear entire LCD screen
#define LCD_CMD_ALL_ON_SCREEN		0x04		// Set all pixels on the LCD screen
#define LCD_CMD_CLEAR_EOL			0x05		// Clear a text row [0..7] from specified start & end columns.
#define LCD_CMD_DRAW_GRAPH			0x06		// Draw a 2D graph of the accumulated data (see ID_COLLECT_GRAPH_DATA)
#define LCD_CMD_DRAW_POLAR			0x07		// 
#define LCD_CMD_MONITOR_CAN			0x08
#define LCD_CMD_DRAW_LINE			0x09		// Draw a 2D line any end points.  Not implemented yet.
#define LCD_CMD_DRAW_HORIZONTAL		0x0A		// Specify the Row, Start & End Columns
#define LCD_CMD_DRAW_VERTICAL		0x0B		// Specify the Column, Start & End Rows
#define LCD_CMD_DRAW_ARC			0x0C		// Not Implemented yet
#define LCD_CMD_DRAW_CIRCLE			0x0D		// Not Implemented yet
#define LCD_CMD_DRAW_BOX			0x0E		// Specify (Start & End) Row; Start & End Column
#define LCD_CMD_INVERT_REGION		0x0F		// Invert Region
#define LCD_CMD_SET_WINDOW			0x10
#define LCD_CMD_BEEP				0x20		// Specify On & Off time in milliseconds
#define LCD_CMD_BACKLIGHT			0x30		// Specify the percent * 10 in word.

void can_prep_test_backlight( sCAN* mMsg, word mValue )
{
    mMsg->id 	  = create_CAN_eid( 0x00, ID_LCD_COMMAND, MyInstance );
	mMsg->data[0] = LCD_CMD_BACKLIGHT;
	float duty    = ((float)mValue / 1024.0)*1000.;
	word dutyW = duty;
	mMsg->data[1] = hi(dutyW);
	mMsg->data[2] = lo(dutyW);
    mMsg->header.DLC = 3;
    mMsg->header.rtr = 0;
}
void can_prep_lcd_beep( sCAN* mMsg, word mValue )
{   
    mMsg->id 	  = create_CAN_eid( 0x00, ID_LCD_COMMAND, MyInstance );
	mMsg->data[0] = LCD_CMD_BEEP;
	mMsg->data[1] = hi(mValue);
	mMsg->data[2] = lo(mValue);
	mMsg->data[3] = 3;
    mMsg->header.DLC = 4;
    mMsg->header.rtr = 0;
}
// Extract the PWM word array; Updates PWM array
void can_prep_pwm_msg( sCAN* mMsg, word mPWM )
{
   The MSG :
		Data is not packed.  Each PWM occupies full 2 bytes.  However the unused upper
		nibble of the first byte contains the index of PWM groups.
		(data[0] & 0xF0)  contains the index of PWM channel groups.  ie.  Channels:
		0..3	nFFF  0FFF  0FFF 0FFF
		4..7		8..11
		12..15		16..18		19..22		23..26
		27..30		31..34		35..38		39..42
		43..46
		47..48	nFFF 0FFF ] end.   
    mMsg->id 	  = create_CAN_eid( 0x00, ID_PWM_CHANNEL_UPDATE, MyInstance );
    mMsg->header.DLC = 8;
    mMsg->header.rtr = 0;
	byte upper_nibble 		 = ((mPWM & 0x0F00)>>8);
	byte lower_nibble 		 = ((mPWM & 0x00FF));
	byte first_index         =  0;
	mMsg->data[0] 			 = ((first_index<<4) | upper_nibble);
	mMsg->data[1] 			 = lower_nibble;
	mMsg->data[2]			 = upper_nibble;
	mMsg->data[3] 			 = lower_nibble;
	mMsg->data[4] 			 = upper_nibble;
	mMsg->data[5] 			 = lower_nibble;
	mMsg->data[6] 			 = upper_nibble;
	mMsg->data[7] 			 = lower_nibble;	
}

