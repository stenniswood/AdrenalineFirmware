#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "global.h"


#ifndef NUM_MOTORS 
#define NUM_MOTORS 5
#endif


/* Q1 Signals */
#define ENCODER_G1_PORT		PORTD
#define ENCODER_G1_PIN		PIND
#define ENCODER_G1_DDR		DDRD
#define ENCODER_G1_MASK		0x0F

#define ENCODER_G3_PORT		PORTE
#define ENCODER_G3_PIN		PINE
#define ENCODER_G3_DDR		DDRE
#define ENCODER_G3_MASK		0x10

/* Q2 Signals */
#define ENCODER_G2_PORT		PORTF
#define ENCODER_G2_PIN		PINF
#define ENCODER_G2_DDR		DDRF
#define ENCODER_G2_MASK		0x1F


//===========================================
// INT7
#define ENCODER_1Q1_PORT	PORTD
#define ENCODER_1Q1_PIN		PIND
#define ENCODER_1Q1_DDR		DDRD
#define ENCODER_1Q1_MASK	(1<<7)

#define ENCODER_1Q2_PORT	PORTF
#define ENCODER_1Q2_PIN		PINF
#define ENCODER_1Q2_DDR		DDRF
#define ENCODER_1Q2_MASK	(1<<0)

// INT5
#define ENCODER_2Q1_PORT	PORTD
#define ENCODER_2Q1_PIN		PIND
#define ENCODER_2Q1_DDR		DDRD
#define ENCODER_2Q1_MASK	(1<<1)

#define ENCODER_2Q2_PORT	PORTF
#define ENCODER_2Q2_PIN		PINF
#define ENCODER_2Q2_DDR		DDRF
#define ENCODER_2Q2_MASK	(1<<1)

// INT1
#define ENCODER_3Q1_PORT	PORTD
#define ENCODER_3Q1_PIN		PIND
#define ENCODER_3Q1_DDR		DDRD
#define ENCODER_3Q1_MASK	(1<<1)

#define ENCODER_3Q2_PORT	PORTF
#define ENCODER_3Q2_PIN		PINF
#define ENCODER_3Q2_DDR		DDRF
#define ENCODER_3Q2_MASK	(1<<2)

// INT0
#define ENCODER_4Q1_PORT	PORTD
#define ENCODER_4Q1_PIN		PIND
#define ENCODER_4Q1_DDR		DDRD
#define ENCODER_4Q1_MASK	(1<<0)

#define ENCODER_4Q2_PORT	PORTF
#define ENCODER_4Q2_PIN		PINF
#define ENCODER_4Q2_DDR		DDRF
#define ENCODER_4Q2_MASK	(1<<3)

// INT4
#define ENCODER_5Q1_PORT	PORTE
#define ENCODER_5Q1_PIN		PINE
#define ENCODER_5Q1_DDR		DDRE
#define ENCODER_5Q1_MASK	(1<<4)

#define ENCODER_5Q2_PORT	PORTF
#define ENCODER_5Q2_PIN		PINF
#define ENCODER_5Q2_DDR		DDRF
#define ENCODER_5Q2_MASK	(1<<4)


struct stEncoder
{
	uint32_t Count;
	uint32_t Speed;	
	uint32_t Acceleration;	
	uint32_t Direction;		// last 

	uint32_t  CountPrev;		// used for Speed
	uint32_t  SpeedPrev;		// used for acceleration
};

extern struct stEncoder Encoders[NUM_MOTORS];



void 	  encoder_init();
void 	  encoder_timeslice();


#endif
