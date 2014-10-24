
void lcd_draw_word	 ( word mData );
void lcd_draw_byte	 ( byte mData );
byte get_ascii_nibble( byte mNib );



void Char_Out		 (char ascii);

void Text_Out_Progmem(_far char* string );
void Text_Out		 (_far char* string );
void Text_Ln_Progmem (_far char* string	);
void Text_Ln		 (_far char* string	);

void Draw_Char_At	(int Row, int Column, int char_index);
void DrawAll		(					);
void Draw_Sample	(					);
byte CenterText		( byte mNumChars, byte mStartCol=0, byte mEndCol=127);

void DrawCharSet	();



