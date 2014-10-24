/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Text Character Generator and Drawing functions  	 *
 *											  						 *
 *	  CATEGORY:   BIOS Driver										 *
 *											  						 *
 *    AUTHOR: Stephen Tenniswood, Software Engineer 	 			 *
 *			  May 7, 2007						 				 	 *
 *											   						 *
 *    PURPOSE: To draw text charactors on bitmap only LCD			 *
 *			 These routines define cursor coordinates independant 	 *
 *			 of the two controllers and their coordinates			 *
 *																	 *
 *			 For the 128x64 LCD, this resuls in a 10x4 line display	 *
 *  		 The character set is 12x16 bitmap chars. Bitmaps were 	 *
 *  		 obtained from 											 *
 * http://www.piclist.com/techref/datafile/dCharSet/extractor/font6x8pic8.asm.txt
 *																	 *
 *********************************************************************/
#include "pgmspace.h"
#include "pin_definitions.h"

#include "lcd_icons.h"
#include "LCD_Driver.h"
#include "lcd_text_6x8_chars.h"

// Char set contains 96 chars.  6 bytes wide.

const byte dCharSet[96][24] PROGMEM = {
//Top half of letters.,,,,,,,,,,,,bottom,,,,,,,,,,,
// first 12 bytes:  top line,
// second 12     :  bottom line
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 0
{ 0x00,0x00,0x00,0x00,0x7C,0xFF,0xFF,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,0x00},	// 1
{ 0x00,0x00,0x00,0x3C,0x3C,0x00,0x00,0x3C,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 2
{ 0x00,0x00,0x10,0x90,0xF0,0x7E,0x1E,0x90,0xF0,0x7E,0x1E,0x10,0x00,0x02,0x1E,0x1F,0x03,0x02,0x1E,0x1F,0x03,0x02,0x00,0x00},	// 3
{ 0x00,0x00,0x78,0xFC,0xCC,0xFF,0xFF,0xCC,0xCC,0x88,0x00,0x00,0x00,0x00,0x04,0x0C,0x0C,0x3F,0x3F,0x0C,0x0F,0x07,0x00,0x00},	// 4
{ 0x00,0x00,0x38,0x38,0x38,0x00,0x80,0xC0,0xE0,0x70,0x38,0x1C,0x00,0x30,0x38,0x1C,0x0E,0x07,0x03,0x01,0x38,0x38,0x38,0x00},	// 5
{ 0x00,0x00,0x00,0xB8,0xFC,0xC6,0xE2,0x3E,0x1C,0x00,0x00,0x00,0x00,0x00,0x1F,0x3F,0x31,0x21,0x37,0x1E,0x1C,0x36,0x22,0x00},	// 6
{ 0x00,0x00,0x00,0x00,0x27,0x3F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 7
{ 0x00,0x00,0x00,0xF0,0xFC,0xFE,0x07,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x0F,0x1F,0x38,0x20,0x20,0x00,0x00,0x00},	// 8
{ 0x00,0x00,0x00,0x01,0x01,0x07,0xFE,0xFC,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x38,0x1F,0x0F,0x03,0x00,0x00,0x00},	// 9
{ 0x00,0x00,0x98,0xB8,0xE0,0xF8,0xF8,0xE0,0xB8,0x98,0x00,0x00,0x00,0x00,0x0C,0x0E,0x03,0x0F,0x0F,0x03,0x0E,0x0C,0x00,0x00},	// 10
{ 0x00,0x00,0x80,0x80,0x80,0xF0,0xF0,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x0F,0x0F,0x01,0x01,0x01,0x00,0x00},	// 11
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xB8,0xF8,0x78,0x00,0x00,0x00,0x00,0x00},	// 12
{ 0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00},	// 13
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x38,0x38,0x00,0x00,0x00,0x00,0x00},	// 14
{ 0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0x70,0x38,0x1C,0x0E,0x00,0x18,0x1C,0x0E,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00},	// 15
{ 0x00,0xF8,0xFE,0x06,0x03,0x83,0xC3,0x63,0x33,0x1E,0xFE,0xF8,0x00,0x07,0x1F,0x1E,0x33,0x31,0x30,0x30,0x30,0x18,0x1F,0x07},	// 16
{ 0x00,0x00,0x00,0x0C,0x0C,0x0E,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x30,0x3F,0x3F,0x30,0x30,0x30,0x00},
{ 0x00,0x1C,0x1E,0x07,0x03,0x03,0x83,0xC3,0xE3,0x77,0x3E,0x1C,0x00,0x30,0x38,0x3C,0x3E,0x37,0x33,0x31,0x30,0x30,0x30,0x30},
{ 0x00,0x0C,0x0E,0x07,0xC3,0xC3,0xC3,0xC3,0xC3,0xE7,0x7E,0x3C,0x00,0x0C,0x1C,0x38,0x30,0x30,0x30,0x30,0x30,0x39,0x1F,0x0E},
{ 0x00,0xC0,0xE0,0x70,0x38,0x1C,0x0E,0x07,0xFF,0xFF,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x3F,0x3F,0x03,0x03},
{ 0x00,0x3F,0x7F,0x63,0x63,0x63,0x63,0x63,0x63,0xE3,0xC3,0x83,0x00,0x0C,0x1C,0x38,0x30,0x30,0x30,0x30,0x30,0x38,0x1F,0x0F},
{ 0x00,0xC0,0xF0,0xF8,0xDC,0xCE,0xC7,0xC3,0xC3,0xC3,0x80,0x00,0x00,0x0F,0x1F,0x39,0x30,0x30,0x30,0x30,0x30,0x39,0x1F,0x0F},
{ 0x00,0x03,0x03,0x03,0x03,0x03,0x03,0xC3,0xF3,0x3F,0x0F,0x03,0x00,0x00,0x00,0x00,0x30,0x3C,0x0F,0x03,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0xBC,0xFE,0xE7,0xC3,0xC3,0xC3,0xE7,0xFE,0xBC,0x00,0x00,0x0F,0x1F,0x39,0x30,0x30,0x30,0x30,0x30,0x39,0x1F,0x0F},
{ 0x00,0x3C,0x7E,0xE7,0xC3,0xC3,0xC3,0xC3,0xC3,0xE7,0xFE,0xFC,0x00,0x00,0x00,0x30,0x30,0x30,0x38,0x1C,0x0E,0x07,0x03,0x00},
{ 0x00,0x00,0x00,0x00,0x70,0x70,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x1C,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x70,0x70,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9C,0xFC,0x7C,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0xC0,0xE0,0xF0,0x38,0x1C,0x0E,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x0E,0x1C,0x38,0x30,0x00,0x00},
{ 0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x00},
{ 0x00,0x00,0x03,0x07,0x0E,0x1C,0x38,0xF0,0xE0,0xC0,0x00,0x00,0x00,0x00,0x30,0x38,0x1C,0x0E,0x07,0x03,0x01,0x00,0x00,0x00},
{ 0x00,0x1C,0x1E,0x07,0x03,0x83,0xC3,0xE3,0x77,0x3E,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x37,0x37,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0xF8,0xFE,0x07,0xF3,0xFB,0x1B,0xFB,0xFB,0x07,0xFE,0xF8,0x00,0x0F,0x1F,0x18,0x33,0x37,0x36,0x37,0x37,0x36,0x03,0x01},
{ 0x00,0x00,0x00,0xE0,0xFC,0x1F,0x1F,0xFC,0xE0,0x00,0x00,0x00,0x00,0x38,0x3F,0x07,0x06,0x06,0x06,0x06,0x07,0x3F,0x38,0x00},
{ 0x00,0xFF,0xFF,0xC3,0xC3,0xC3,0xC3,0xE7,0xFE,0xBC,0x00,0x00,0x00,0x3F,0x3F,0x30,0x30,0x30,0x30,0x30,0x39,0x1F,0x0F,0x00},
{ 0x00,0xF0,0xFC,0x0E,0x07,0x03,0x03,0x03,0x07,0x0E,0x0C,0x00,0x00,0x03,0x0F,0x1C,0x38,0x30,0x30,0x30,0x38,0x1C,0x0C,0x00},
{ 0x00,0xFF,0xFF,0x03,0x03,0x03,0x03,0x07,0x0E,0xFC,0xF0,0x00,0x00,0x3F,0x3F,0x30,0x30,0x30,0x30,0x38,0x1C,0x0F,0x03,0x00},
{ 0x00,0xFF,0xFF,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0x03,0x03,0x00,0x00,0x3F,0x3F,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x00},
{ 0x00,0xFF,0xFF,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0x03,0x03,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0xF0,0xFC,0x0E,0x07,0x03,0xC3,0xC3,0xC3,0xC7,0xC6,0x00,0x00,0x03,0x0F,0x1C,0x38,0x30,0x30,0x30,0x30,0x3F,0x3F,0x00},
{ 0x00,0xFF,0xFF,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xFF,0xFF,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x00},
{ 0x00,0x00,0x00,0x03,0x03,0xFF,0xFF,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x3F,0x3F,0x30,0x30,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x0E,0x1E,0x38,0x30,0x30,0x30,0x30,0x38,0x1F,0x07,0x00},
{ 0x00,0xFF,0xFF,0xC0,0xE0,0xF0,0x38,0x1C,0x0E,0x07,0x03,0x00,0x00,0x3F,0x3F,0x00,0x01,0x03,0x07,0x0E,0x1C,0x38,0x30,0x00},
{ 0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x00},
{ 0x00,0xFF,0xFF,0x1E,0x78,0xE0,0xE0,0x78,0x1E,0xFF,0xFF,0x00,0x00,0x3F,0x3F,0x00,0x00,0x01,0x01,0x00,0x00,0x3F,0x3F,0x00},
{ 0x00,0xFF,0xFF,0x0E,0x38,0xF0,0xC0,0x00,0x00,0xFF,0xFF,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x03,0x07,0x1C,0x3F,0x3F,0x00},
{ 0x00,0xF0,0xFC,0x0E,0x07,0x03,0x03,0x07,0x0E,0xFC,0xF0,0x00,0x00,0x03,0x0F,0x1C,0x38,0x30,0x30,0x38,0x1C,0x0F,0x03,0x00},
{ 0x00,0xFF,0xFF,0x83,0x83,0x83,0x83,0x83,0xC7,0xFE,0x7C,0x00,0x00,0x3F,0x3F,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00},
{ 0x00,0xF0,0xFC,0x0E,0x07,0x03,0x03,0x07,0x0E,0xFC,0xF0,0x00,0x00,0x03,0x0F,0x1C,0x38,0x30,0x36,0x3E,0x1C,0x3F,0x33,0x00},
{ 0x00,0xFF,0xFF,0x83,0x83,0x83,0x83,0x83,0xC7,0xFE,0x7C,0x00,0x00,0x3F,0x3F,0x01,0x01,0x03,0x07,0x0F,0x1D,0x38,0x30,0x00},
{ 0x00,0x3C,0x7E,0xE7,0xC3,0xC3,0xC3,0xC3,0xC7,0x8E,0x0C,0x00,0x00,0x0C,0x1C,0x38,0x30,0x30,0x30,0x30,0x39,0x1F,0x0F,0x00},
{ 0x00,0x00,0x03,0x03,0x03,0xFF,0xFF,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x07,0x1F,0x38,0x30,0x30,0x30,0x30,0x38,0x1F,0x07,0x00},
{ 0x00,0x07,0x3F,0xF8,0xC0,0x00,0x00,0xC0,0xF8,0x3F,0x07,0x00,0x00,0x00,0x00,0x01,0x0F,0x3E,0x3E,0x0F,0x01,0x00,0x00,0x00},
{ 0x00,0xFF,0xFF,0x00,0x00,0x80,0x80,0x00,0x00,0xFF,0xFF,0x00,0x00,0x3F,0x3F,0x1C,0x06,0x03,0x03,0x06,0x1C,0x3F,0x3F,0x00},
{ 0x00,0x03,0x0F,0x1C,0x30,0xE0,0xE0,0x30,0x1C,0x0F,0x03,0x00,0x00,0x30,0x3C,0x0E,0x03,0x01,0x01,0x03,0x0E,0x3C,0x30,0x00},
{ 0x00,0x03,0x0F,0x3C,0xF0,0xC0,0xC0,0xF0,0x3C,0x0F,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x03,0x03,0x03,0x03,0xC3,0xE3,0x33,0x1F,0x0F,0x03,0x00,0x00,0x30,0x3C,0x3E,0x33,0x31,0x30,0x30,0x30,0x30,0x30,0x00},
{ 0x00,0x00,0x00,0xFF,0xFF,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x30,0x30,0x30,0x30,0x00,0x00,0x00},
{ 0x00,0x0E,0x1C,0x38,0x70,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x0E,0x1C,0x18},
{ 0x00,0x00,0x00,0x03,0x03,0x03,0x03,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x30,0x30,0x3F,0x3F,0x00,0x00,0x00},
{ 0x00,0x60,0x70,0x38,0x1C,0x0E,0x07,0x0E,0x1C,0x38,0x70,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0},
{ 0x00,0x00,0x00,0x00,0x00,0x3E,0x7E,0x4E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x40,0x60,0x60,0x60,0x60,0x60,0x60,0xE0,0xC0,0x00,0x00,0x1C,0x3E,0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x3F,0x00},
{ 0x00,0xFF,0xFF,0xC0,0x60,0x60,0x60,0x60,0xE0,0xC0,0x80,0x00,0x00,0x3F,0x3F,0x30,0x30,0x30,0x30,0x30,0x38,0x1F,0x0F,0x00},
{ 0x00,0x80,0xC0,0xE0,0x60,0x60,0x60,0x60,0x60,0xC0,0x80,0x00,0x00,0x0F,0x1F,0x38,0x30,0x30,0x30,0x30,0x30,0x18,0x08,0x00},
{ 0x00,0x80,0xC0,0xE0,0x60,0x60,0x60,0xE0,0xC0,0xFF,0xFF,0x00,0x00,0x0F,0x1F,0x38,0x30,0x30,0x30,0x30,0x30,0x3F,0x3F,0x00},
{ 0x00,0x80,0xC0,0xE0,0x60,0x60,0x60,0x60,0x60,0xC0,0x80,0x00,0x00,0x0F,0x1F,0x3B,0x33,0x33,0x33,0x33,0x33,0x13,0x01,0x00},
{ 0x00,0xC0,0xC0,0xFC,0xFE,0xC7,0xC3,0xC3,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x80,0xC0,0xE0,0x60,0x60,0x60,0x60,0x60,0xE0,0xE0,0x00,0x00,0x03,0xC7,0xCE,0xCC,0xCC,0xCC,0xCC,0xE6,0x7F,0x3F,0x00},
{ 0x00,0xFF,0xFF,0xC0,0x60,0x60,0x60,0xE0,0xC0,0x80,0x00,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x60,0xEC,0xEC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x3F,0x3F,0x30,0x30,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xEC,0xEC,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xE0,0xC0,0xC0,0xFF,0x7F,0x00,0x00,0x00},
{ 0x00,0x00,0xFF,0xFF,0x00,0x80,0xC0,0xE0,0x60,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x03,0x07,0x0F,0x1C,0x38,0x30,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x03,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x3F,0x3F,0x30,0x30,0x00,0x00,0x00},
{ 0x00,0xE0,0xC0,0xE0,0xE0,0xC0,0xC0,0xE0,0xE0,0xC0,0x80,0x00,0x00,0x3F,0x3F,0x00,0x00,0x3F,0x3F,0x00,0x00,0x3F,0x3F,0x00},
{ 0x00,0x00,0xE0,0xE0,0x60,0x60,0x60,0x60,0xE0,0xC0,0x80,0x00,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x00},
{ 0x00,0x80,0xC0,0xE0,0x60,0x60,0x60,0x60,0xE0,0xC0,0x80,0x00,0x00,0x0F,0x1F,0x38,0x30,0x30,0x30,0x30,0x38,0x1F,0x0F,0x00},
{ 0x00,0xE0,0xE0,0x60,0x60,0x60,0x60,0x60,0xE0,0xC0,0x80,0x00,0x00,0xFF,0xFF,0x0C,0x18,0x18,0x18,0x18,0x1C,0x0F,0x07,0x00},
{ 0x00,0x80,0xC0,0xE0,0x60,0x60,0x60,0x60,0x60,0xE0,0xE0,0x00,0x00,0x07,0x0F,0x1C,0x18,0x18,0x18,0x18,0x0C,0xFF,0xFF,0x00},
{ 0x00,0x00,0xE0,0xE0,0xC0,0x60,0x60,0x60,0x60,0xE0,0xC0,0x00,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0xC0,0xE0,0x60,0x60,0x60,0x60,0x60,0x40,0x00,0x00,0x00,0x00,0x11,0x33,0x33,0x33,0x33,0x33,0x3F,0x1E,0x00,0x00,0x00},
{ 0x00,0x60,0x60,0xFE,0xFE,0x60,0x60,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x3F,0x30,0x30,0x30,0x30,0x00,0x00,0x00},
{ 0x00,0xE0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xE0,0x00,0x00,0x0F,0x1F,0x38,0x30,0x30,0x30,0x30,0x18,0x3F,0x3F,0x00},
{ 0x00,0x60,0xE0,0x80,0x00,0x00,0x00,0x00,0x80,0xE0,0x60,0x00,0x00,0x00,0x01,0x07,0x1E,0x38,0x38,0x1E,0x07,0x01,0x00,0x00},
{ 0x00,0xE0,0xE0,0x00,0x00,0xE0,0xE0,0x00,0x00,0xE0,0xE0,0x00,0x00,0x07,0x1F,0x38,0x1C,0x0F,0x0F,0x1C,0x38,0x1F,0x07,0x00},
{ 0x00,0x60,0xE0,0xC0,0x80,0x00,0x80,0xC0,0xE0,0x60,0x00,0x00,0x00,0x30,0x38,0x1D,0x0F,0x07,0x0F,0x1D,0x38,0x30,0x00,0x00},
{ 0x00,0x00,0x60,0xE0,0x80,0x00,0x00,0x80,0xE0,0x60,0x00,0x00,0x00,0x00,0x00,0x81,0xE7,0x7E,0x1E,0x07,0x01,0x00,0x00,0x00},
{ 0x00,0x60,0x60,0x60,0x60,0x60,0xE0,0xE0,0x60,0x20,0x00,0x00,0x00,0x30,0x38,0x3C,0x36,0x33,0x31,0x30,0x30,0x30,0x00,0x00},
{ 0x00,0x00,0x80,0xC0,0xFC,0x7E,0x07,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x01,0x1F,0x3F,0x70,0x60,0x60,0x60,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x03,0x03,0x03,0x07,0x7E,0xFC,0xC0,0x80,0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x70,0x3F,0x1F,0x01,0x00,0x00,0x00},
{ 0x00,0x10,0x18,0x0C,0x04,0x0C,0x18,0x10,0x18,0x0C,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x80,0xC0,0x60,0x30,0x30,0x60,0xC0,0x80,0x00,0x00,0x00,0x0F,0x0F,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0F,0x0F,0x00}
};

#define CHAR_WIDTH 12
	
void EOL_WrapAround_Check()
{
	byte y_address = Get_y_address();
	byte x_address = Get_x_address();
	
	// do EOL carriage return 
	if (y_address > 115) 
	{ 			
		// Move back to start of line:
		Set_y_address( 0 );
				
		// Goto Next Line
		if (x_address >= 5) 
			x_address = 0;
		else
			x_address += 2;
			
		Set_x_address( x_address );
		SendAddresses();
	}	
}
	
// DRAWS SINGLE (LARGE FONT) CHARACTER AT CURRENT CURSOR POSITION
void dDraw_Char(int char_index)
{
	byte save_y;
	 _far char* ptr = 0;	// was const

	//EOL_WrapAround_Check();
	save_y = Get_y_address();

	ptr = (char*)dCharSet[char_index];
	TransferBufferFixedLength(ptr, CHAR_WIDTH);	
	
	// REPOSITION FOR BOTTOM HALF
	Increment_x_address();
	Set_y_address(save_y);
	SendAddresses();
	
	// DRAW BOTTOM HALF
	ptr+= CHAR_WIDTH;
	TransferBufferFixedLength(ptr, CHAR_WIDTH);
	
	// ADJUST POSITION FOR NEXT CHARACTER ON SAME LINE:
	Set_x_address( Get_x_address()-1 );
	SendAddresses();
};

// Draws the ASCII Character:
void dChar_Out(char ascii)
{
	byte index = ascii - 32;
	dDraw_Char(index);
};

// Draws a String on LCD at current position.
void dText_Out(_far char* string )
{
	int i=0;
	while (string[i] != 0)
	{
		dChar_Out(string[i++]);
	};
};

// Draws a String and returns to next line.
void dText_Ln(_far char* string )
{
	dText_Out(string);

	Increment_x_address();
	Increment_x_address();
	Set_y_address (0);
	SendAddresses();
};

// Draws char at specified position.
// Inorder to be compatible with smaller characters,
// the following 
// WORKS IN MULTIPLES OF 6 in Y_ADDRESS,
// AND                   1 in X_ADDRESS
// Row is the line number [0..7]
// Column is the column [0..20]
void dDraw_Char_At(int Row, int Column, char char_index)
{
	if (Row > 7) 	Row=7;
	if (Column>20) 	Column=20;

	Set_x_address( Row );
	Set_y_address( Column*6+2 );
	dChar_Out(char_index);
};


byte dCenterLargeText( byte mNumChars, byte mStartCol, byte mEndCol)
{
	word OccupiedX = mNumChars*12;	
	word StartX = (mEndCol - mStartCol-OccupiedX)>>1;
	return StartX;	
}

// For test purposes, draws a row of chars on LCD.
void DrawdCharSet()
{	
	int index=0;
	GotoAddress (0,0);
	for (index = 0; index < 21; index++)
	   dDraw_Char( index  );
	   
	GotoAddress (1,0);
	for (; index < 42; index++)
	   dDraw_Char( index  );

	GotoAddress (2,0);
	for (; index < 63; index++)
	   dDraw_Char( index  );

	GotoAddress (3,0);
	for (; index < 84; index++)
	   dDraw_Char( index  );

	GotoAddress (4,0);
	for (; index < 97; index++)
	   dDraw_Char( index  );
}


