#ifndef _ARIAL_BOLD_14_
#define _ARIAL_BOLD_14_

// Font Indices
#define FONT_LENGTH			0
#define FONT_FIXED_WIDTH	2
#define FONT_HEIGHT			3
#define FONT_FIRST_CHAR		4
#define FONT_CHAR_COUNT		5
#define FONT_WIDTH_TABLE	6

// the following returns true if the given font is fixed width
// zero length is flag indicating fixed width font (array does not contain width data entries)
#define isFixedWidthFont(font)  (FontRead(font+FONT_LENGTH) == 0 && FontRead(font+FONT_LENGTH+1) == 0))

extern uint8_t Arial_bold_14[];

typedef const uint8_t* Font_t;  	

// Colors
#define BLACK				0xFF
#define WHITE				0x00

/// @cond hide_from_doxygen
struct tarea
{
	uint8_t x1;
	uint8_t y1;
	uint8_t	x2;
	uint8_t y2;
	int8_t  mode;
};


void selectFont( Font_t mFont );
int PutChar(uint8_t c);
int aText_Out( char* mString );

#endif
