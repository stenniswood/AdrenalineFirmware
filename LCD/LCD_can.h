#ifndef _LCD_CAN_H_
#define _LCD_CAN_H_


byte get_ascii_nibble( byte mNib );
void lcd_draw_byte	( byte* mData	);
void lcd_draw_word	( byte* mData	);

void show_can_id	( struct sCAN* mCAN	);		// Prints the id to the LCD
void show_can_data	( struct sCAN* mCAN	);		// Prints the data bytes
void show_can_msg	( struct sCAN* mCAN, byte mRow	);		// Formats an prints the entire message
void show_all_can_msgs(	);


#endif
