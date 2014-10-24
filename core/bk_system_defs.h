#ifndef _BK_SYSTEM_H_
#define _BK_SYSTEM_H_

#include <avr/sfr_defs.h>
#include <avr/common.h>


#define byte unsigned char
#define word unsigned short
#define BOOL byte
#define TRUE  1
#define FALSE 0

#ifndef NULL
#define NULL  0
#endif

#define cli()  __asm__ __volatile__ ("cli" ::)
#define sei()  __asm__ __volatile__ ("sei" ::)

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define CRYSTAL_FREQ 16000000
#define F_CPU CRYSTAL_FREQ

#define lo_word(one_long)	(one_long & 0x0000FFFF)
#define hi_word(one_long)	((one_long & 0xFFFF0000) >> 16)
#define lo(one_word) 		(one_word & 0x00FF)
#define hi(one_word) 		((one_word & 0xFF00) >> 8)

union float_array
{
	float value;
	uint8_t array[sizeof(float)];
};

#define one_second 400000
#define STROBE_SPEED 10

void delay( long int mdelay );
void chip_init(void);


#endif
