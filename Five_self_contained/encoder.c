/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Low Level Routines RTOS					 	 	 *
 *											  						 *
 *	  CATEGORY:   BIOS Driver										 *
 *											   						 *
 *    PURPOSE: Use Timer 0 for a 1ms tic.  Interrupt handler to 	 *
 *           set update counters, then dispatch any tasks.			 *
 *																	 *
 *	  HARDWARE RESOURCES:									   		 *
 *			 Atmega16m1 Timer 0 is set up as an 8bit PWM	 		 *
 *																	 *
 *	  PIN ASSIGNMENTS:												 *
 *           No external pins 						 				 * 
 *											   						 *
 *    AUTHOR: Stephen Tenniswood, Robotics Engineer 	 			 *
 *		  	  December 2007						 				 	 *
 *		  	  Revised for atmel Sept 2013						 	 *
 *********************************************************************/
/*********************************************************************
Product of Beyond Kinetics, Inc
This file puts together various other files:  
	pot.c calibrations.c encoder.c can_msg.c 
-------------------------------------------------------------------
DATE 	:  10/20/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "motor.h"
#include "encoder.h"
#include "global.h"


/**************** TYPE DEFS *************************************/
#define CW  0
#define CCW 1

/**************** VARIABLES *************************************/
struct stEncoder Encoders[NUM_MOTORS];


/*  We'll be using INT0,INT1,INT2,INT3, and INT4 for the 5 encoder interrupts.
	These correspond to the Q1 inputs.	
*/
ISR ( INT0_vect )
{
	const int e=3;
	byte Q2 = (ENCODER_4Q2_PIN & ENCODER_4Q2_MASK);

	if (Q2==1) Encoders[e].Direction = CW;
	else       Encoders[e].Direction = CCW;
	
	if (Encoders[e].Direction == CW)
		Encoders[e].Count++;		// never reset.  Holds _THE_ count from min stop.
	else 
		Encoders[e].Count--;
	// EIFR   |= 0x01;   // Not necessary 
}

ISR ( INT1_vect )
{
	const int e=2;
	byte Q2 = (ENCODER_3Q2_PIN & ENCODER_3Q2_MASK);

	if (Q2==1) Encoders[e].Direction = CW;
	else       Encoders[e].Direction = CCW;
	
	if (Encoders[e].Direction == CW)
		Encoders[e].Count++;		// never reset.  Holds _THE_ count from min stop.
	else 
		Encoders[e].Count--;
}

ISR ( INT2_vect )
{
	const int e=3;
	byte Q2 = (ENCODER_1Q2_PIN & ENCODER_1Q2_MASK);

	if (Q2==1) Encoders[e].Direction = CW;
	else       Encoders[e].Direction = CCW;
	
	if (Encoders[e].Direction == CW)
		Encoders[e].Count++;		// never reset.  Holds _THE_ count from min stop.
	else 
		Encoders[e].Count--;
}

ISR ( INT3_vect )
{
	const int e=3;
	byte Q2 = (ENCODER_2Q2_PIN & ENCODER_2Q2_MASK);

	if (Q2==1) Encoders[e].Direction = CW;
	else       Encoders[e].Direction = CCW;
	
	if (Encoders[e].Direction == CW)
		Encoders[e].Count++;		// never reset.  Holds _THE_ count from min stop.
	else 
		Encoders[e].Count--;
}

ISR ( INT4_vect )
{
	const int e=3;
	byte Q2 = (ENCODER_5Q2_PIN & ENCODER_5Q2_MASK);

	if (Q2==1) Encoders[e].Direction = CW;
	else       Encoders[e].Direction = CCW;
	
	if (Encoders[e].Direction == CW)
		Encoders[e].Count++;		// never reset.  Holds _THE_ count from min stop.
	else 
		Encoders[e].Count--;
}


void encoder_init()
{
	ENCODER_G1_DDR	&= ~(ENCODER_G1_MASK);
	ENCODER_G1_PORT	&= ~(ENCODER_G1_MASK);		// pull-ups

	ENCODER_G2_DDR	&= ~(ENCODER_G2_MASK);
	ENCODER_G2_PORT	&= ~(ENCODER_G2_MASK);		// pull-ups

	ENCODER_G3_DDR	&= ~(ENCODER_G3_MASK);
	ENCODER_G3_PORT	&= ~(ENCODER_G3_MASK);		// pull-ups

	// Enable Interrupts on Rising Edges:
//	EIFR  |= 0x1F;   	// clear all flag bits
	EIMSK |= 0x1F;		// Enable ints: INT0..4
	EICRA  = 0xFF;		// Rising edges ( int0..3 )
	EICRB |= 0x03;		// Rising edges.	
}

/* Call every ~20ms ==> 50hz */
// Counts per Second : 
void encoder_timeslice()
{	
	for (int i=0; i<NUM_MOTORS; i++) {
		Encoders[i].Speed 		 = (Encoders[i].Count - Encoders[i].CountPrev);
		Encoders[i].Acceleration = (Encoders[i].Speed - Encoders[i].SpeedPrev);		// counts per second per second
	}

	for (int i=0; i<NUM_MOTORS; i++) {
		Encoders[i].CountPrev = Encoders[i].Count;
		Encoders[i].SpeedPrev = Encoders[i].Speed;
	}
}

