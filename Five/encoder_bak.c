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


/**************** TYPE DEFS *************************************/
#define CW  0
#define CCW 1

/**************** VARIABLES *************************************/
word  EncoderCount;
float EncoderSpeed;
float EncoderAcceleration;

byte prev_q1 = 0;		// Holds the previous sample of the Q1 quadrature input.
byte prev_q2 = 0;		// Holds the previous sample of the Q1 quadrature input.
byte prev_direction = CW;

// This covers PCint pins[7:0]
ISR ( PCINT0_vect )
{
	byte q1 = (ENCODER_PORT & (1<<ENCODER_Q1));
	byte q2 = (ENCODER_PORT & (1<<ENCODER_Q2));
	byte Direction = CW;
	
	// Decode Direction & Count!		
	// Determine direction by the value of Q2 when Q1 transitions high.
	if ((prev_q1==0) && (q1>0))		// Low to High
	{
		if (q2==1) Direction = CW;
		if (q2==0) Direction = CCW;
		if (Direction == CW)
			EncoderCount++;		// never reset.  Holds _THE_ count from min stop.
		else 
			EncoderCount--;
	}
}

void encoder_init()
{
	// SET INPUTS:
	ENCODER_DDR |= ((1<<ENCODER_Q1) | (1<<ENCODER_Q2));	
}

/* Call every ~20ms */
byte prev_Count  = 0;
float prev_Speed = 0;

void encoder_timeslice()
{
	// Counts per Second : 
	EncoderSpeed 		= (EncoderCount - prev_Count) * 50;		// for 10ms task : *100
	EncoderAcceleration = (EncoderSpeed - prev_Speed) * 50;		// counts per second per second
	
	prev_Count = EncoderCount;
	prev_Speed = EncoderSpeed;			
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






