#ifndef _CANI_MSG_H_
#define _CANI_MSG_H_

/* All of the following Commands go under 1 CAN message ID - "ID_LCD_COMMAND"
The first data[0] element determines the required action.
The following is a list of accepted functions.

FORMAT FOR ID_LCD_COMMAND:
data[0] = 0=No action; 1=MoveTo; 2=ClearScreen; 3=Clear EOL; 4=Draw Graph; 5=Draw Polar; 6=Monitor CAN_ID_LIST_h
		  7=Draw Line; 8=DrawHorizontal; 9=DrawVertical; A=Draw Arc; B=DrawCircle; C= Draw Box;
		  D=Invert Region; 		  x10= Set Window; 		  x20= Beep;		  x30= BackLight;
data[1] = Cursor X 
data[2] = Cursor Y 
data[3] = Font Selection (0=6x8; 1=12x16; 2=Arial14; FF=NoChange;)  */
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
const long SerialNumber      	= 0x56789CDE;


void can_file_message		  ( struct sCAN* mMsg );
void can_proc_lcd_command_msg ( struct sCAN* mMsg );
void can_proc_draw_text_msg	  ( struct sCAN* mMsg );
void can_proc_collect_data_msg( struct sCAN* mMsg );

void can_prep_board_id_msg	  ( struct sCAN* mMsg );
void can_init_test_msg();


#endif
