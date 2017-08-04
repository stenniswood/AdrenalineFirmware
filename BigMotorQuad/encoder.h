#ifndef _ENCODER_H_
#define _ENCODER_H_


#include "global.h"

#define ENCODER_G1_PORT		PORTE
#define ENCODER_G1_PIN		PINE
#define ENCODER_G1_DDR		DDRE
#define ENCODER_G1_MASK		0xF0

#define ENCODER_G2_PORT		PORTD
#define ENCODER_G2_PIN		PIND
#define ENCODER_G2_DDR		DDRD
#define ENCODER_G2_MASK		0x33

//===========================================
// INT7
#define ENCODER_1Q1_PORT	PORTE
#define ENCODER_1Q1_PIN		PINE
#define ENCODER_1Q1_DDR		DDRE
#define ENCODER_1Q1_MASK	(1<<7)
#define ENCODER_1Q2_PORT	PORTE
#define ENCODER_1Q2_PIN		PINE
#define ENCODER_1Q2_DDR		DDRE
#define ENCODER_1Q2_MASK	(1<<6)

// INT5
#define ENCODER_2Q1_PORT	PORTE
#define ENCODER_2Q1_PIN		PINE
#define ENCODER_2Q1_DDR		DDRE
#define ENCODER_2Q1_MASK	(1<<5)
#define ENCODER_2Q2_PORT	PORTE
#define ENCODER_2Q2_PIN		PINE
#define ENCODER_2Q2_DDR		DDRE
#define ENCODER_2Q2_MASK	(1<<4)

// INT1
#define ENCODER_3Q1_PORT	PORTD
#define ENCODER_3Q1_PIN		PIND
#define ENCODER_3Q1_DDR		DDRD
#define ENCODER_3Q1_MASK	(1<<1)
#define ENCODER_3Q2_PORT	PORTD
#define ENCODER_3Q2_PIN		PIND
#define ENCODER_3Q2_DDR		DDRD
#define ENCODER_3Q2_MASK	(1<<4)

// INT0
#define ENCODER_4Q1_PORT	PORTD
#define ENCODER_4Q1_PIN		PIND
#define ENCODER_4Q1_DDR		DDRD
#define ENCODER_4Q1_MASK	(1<<0)
#define ENCODER_4Q2_PORT	PORTD
#define ENCODER_4Q2_PIN		PIND
#define ENCODER_4Q2_DDR		DDRD
#define ENCODER_4Q2_MASK	(1<<5)


#define NUM_MOTORS 4
extern uint32_t EncoderCount[NUM_MOTORS];
extern uint32_t EncoderSpeed[NUM_MOTORS];

extern float EncoderAcceleration[NUM_MOTORS];


void 	  encoder_init();
long int  get_encoder_angle();
void 	  encoder_timeslice();


#endif
