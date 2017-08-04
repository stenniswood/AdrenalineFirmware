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
uint32_t  EncoderDirection[4];		// Timeslice exit
uint32_t  EncoderCountPrev[4];		// Timeslice exit
uint32_t  EncoderCount[4];			// Updated in ISR
uint32_t  EncoderSpeed[4];			// counts per second
uint32_t  EncoderSpeedPrev[4];		//
float EncoderAcceleration[4];		// counts per second per second


byte prev_q1 = 0;		// Holds the previous sample of the Q1 quadrature input.
byte prev_q2 = 0;		// Holds the previous sample of the Q1 quadrature input.
byte prev_direction = CW;

/*  We'll be using INT0,INT1,INT7, and INT5 for the 4 encoder interrupts.
	These correspond to the Q1 inputs.
	
*/
ISR ( INT0_vect )
{
	const int e=3;
	byte Q2 = (ENCODER_4Q2_PIN & ENCODER_4Q2_MASK);

	if (Q2==1) EncoderDirection[e] = CW;
	else       EncoderDirection[e] = CCW;
	
	if (EncoderDirection[e] == CW)
		EncoderCount[e]++;		// never reset.  Holds _THE_ count from min stop.
	else 
		EncoderCount[e]--;
}

ISR ( INT1_vect )
{
	const int e=2;
	byte Q2 = (ENCODER_3Q2_PIN & ENCODER_3Q2_MASK);

	if (Q2==1) EncoderDirection[e] = CW;
	else       EncoderDirection[e] = CCW;
	
	if (EncoderDirection[e] == CW)
		EncoderCount[e]++;		// never reset.  Holds _THE_ count from min stop.
	else 
		EncoderCount[e]--;
}

ISR ( INT7_vect )
{
	const int e=3;
	byte Q2 = (ENCODER_1Q2_PIN & ENCODER_1Q2_MASK);

	if (Q2==1) EncoderDirection[e] = CW;
	else       EncoderDirection[e] = CCW;
	
	if (EncoderDirection[e] == CW)
		EncoderCount[e]++;		// never reset.  Holds _THE_ count from min stop.
	else 
		EncoderCount[e]--;
}

ISR ( INT5_vect )
{
	const int e=3;
	byte Q2 = (ENCODER_2Q2_PIN & ENCODER_2Q2_MASK);

	if (Q2==1) EncoderDirection[e] = CW;
	else       EncoderDirection[e] = CCW;
	
	if (EncoderDirection[e] == CW)
		EncoderCount[e]++;		// never reset.  Holds _THE_ count from min stop.
	else 
		EncoderCount[e]--;
}


void encoder_init()
{
	ENCODER_G1_DDR	&= ~(ENCODER_G1_MASK);
	ENCODER_G1_PORT	|= 0xF0;		// pull-ups

	ENCODER_G2_DDR	&= ~(ENCODER_G2_MASK);
	ENCODER_G2_PORT	|= 0xF0;		// pull-ups

	// Enable Interrupts on Rising Edges:
//	PCMSK0 |= ((1<<ENCODER_Q1) | (1<<ENCODER_Q2));	
	PCICR  |= 0x01;     //PCIE0;
	//EIFR   |= 0x01;   //INTF0;
	EIMSK |= 0x01;		//INT0
	EICRA |= 0x03;		// Rising edges.
}

/* Call every ~20ms ==> 50hz */
// Counts per Second : 
void encoder_timeslice()
{	
	for (int i=0; i<4; i++) {
		EncoderSpeed[i] 		= (EncoderCount[i] - EncoderCountPrev[i]) * 50;		// for 10ms task : *100
		EncoderAcceleration[i]  = (EncoderSpeed[i] - EncoderSpeedPrev[i]) * 50;		// counts per second per second
	}
		
	for (int i=0; i<4; i++) {
		EncoderCountPrev[i] = EncoderCount[i];
		EncoderSpeedPrev[i] = EncoderSpeed[i];
	}
}


/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
long int  get_encoder_angle()
{
	// Need to fix!  Verify stops are at least word size.  
	//return convert_to_angle( EncoderCount );
	return 0;
}


