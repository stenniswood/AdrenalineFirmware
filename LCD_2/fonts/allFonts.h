/*
 * allFonts.h font header for GLCD library 
 * The fonts listed below will be available in a sketch if this file is included
 *
 * If you create your own fonts you can add the header to this file
 *
 * Note that the build environment only holds a font in Flash if its selected 
 * so there is no penalty to including a font file here if its not used 
 */

/*#include "SystemFont5x7.h"       // system font
#include "Arial14.h"             // proportional font
#include "Arial_bold_14.h"       // Bold proportional font
#include "Corsiva_12.h"
#include "Verdana_digits_24.h"   // large proportional font - numerals only 
#include "fixednums7x15.h"       // fixed width font - numerals only 
#include "fixednums8x16.h"       // fixed width font - numerals only 
#include "fixednums15x31.h"      // fixed width font - numerals only 
*/

#ifndef ARIAL_14_H
#define ARIAL_14_H
#define ARIAL_14_WIDTH 10
#define ARIAL_14_HEIGHT 14
#define Arial14 Arial_14 
extern uint8_t Arial_14[] ;
#endif

#ifndef ARIAL_BOLD_14_H
#define ARIAL_BOLD_14_H
#define ARIAL_BOLD_14_WIDTH 10
#define ARIAL_BOLD_14_HEIGHT 14
extern uint8_t Arial_bold_14[] ;
#endif

#ifndef CORSIVA_12_H
#define CORSIVA_12_H
#define CORSIVA_12_WIDTH 10
#define CORSIVA_12_HEIGHT 11
extern uint8_t Corsiva_12[] ;
#endif

#ifndef FIXEDNUMS7x15_H
#define FIXEDNUMS7x15_H
extern uint8_t fixednums7x15[] ;
#endif

#ifndef FIXEDNUMS8x16_H
#define FIXEDNUMS8x16_H
extern uint8_t fixednums8x16[] ;
#endif

#ifndef FIXEDNUMS15x31_H
#define FIXEDNUMS15x31_H
extern uint8_t fixednums15x31[] ;
#endif

#ifndef SYSTEM5x7_H
#define SYSTEM5x7_H
#define SYSTEM5x7_WIDTH 5
#define SYSTEM5x7_HEIGHT 7
#define SystemFont5x7 System5x7
extern uint8_t System5x7[] ;
#endif

#ifndef VERDANA24_H
#define VERDANA24_H
#define VERDANA24_WIDTH 10
#define VERDANA24_HEIGHT 24
extern uint8_t Verdana24[] ;
#endif

