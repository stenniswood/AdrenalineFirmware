/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Low Level Routines RTOS					 	 	 *
 *											  						 *
 *	  CATEGORY:   Core Functionality	+ App specific				 *
 *											   						 *
 *    PURPOSE: 	  Sets up the ADC converter with continuous interrupt*
 *				  sampling.										 	 *
 *																	 *
 *	  HARDWARE RESOURCES:									   		 *
 *			 Atmega16m1 ADC									 		 *
 *																	 *
 *	  PIN ASSIGNMENTS:												 *
 *           No external pins 						 				 * 
 *											   						 *
 *    AUTHOR: Stephen Tenniswood, Robotics Engineer 	 			 *
 *		  	  November 2013						 				 	 *
 *		  	  Revised for atmel Sept 2013						 	 *
 * 			  Product of Beyond Kinetics, Inc						 *
 *********************************************************************/
#include <stdlib.h>
#include <math.h>
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_instance.h"
#include "pwm.h"
#include "eeprom.h"
#include "adc.h"

/*********************************************************************************
 EVERY MODULE GETS ADC.O NOW.  IF NOTHING ELSE, USED FOR RANDOM NUMBER GENERATION. 
 **************** RANDOM NUMBER **************************************************/
/*#ifndef USES_ADC
void random_number_adc_init()
{
	byte reg = (1<<ADEN) | (1<<ADIF) | 0x03;
	ADCSRA = reg;
	//	Digital Input Disable (reduce power consumption)
	//  We are using ADC2, ADC8, ADC9:
	DIDR0  |=  (1<<ADC3D);
	// HighSpeed Mode & AREF Enable internal circuitry.
	ADCSRB = (1<<ADHSM);
}
#else*/

//#include "motor.h"
//#include "pot.h"

/**************** VARIABLES *************************************/
word LightSensorSample [2];
byte next_channel = 0;

ISR ( ADC_vect )
{
	word result = ADCL;
	result      += (ADCH<<8);
	switch(next_channel)
	{ 
	case 0:	LightSensorSample[0] = LightSensorSample[1];
			LightSensorSample[1] = result;
			next_channel = 1;
			ADMUX = POT_MUX;		// Select POT	
			ADCSRA |= (1<<ADSC);	// Start conversion
			break;
			
	default: 			
			next_channel = 0;
			ADMUX = POT_MUX;		// Select POT	
			ADCSRA |= (1<<ADSC);	// Start conversion
			break;
	}
	// Clear IF automatically?	
}

/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
/* To SAVE Power - Shut down the ADC block */
void adc_sleep()
{
	// Clear ADEN bit
	ADCSRA &= ~ADEN;
}


/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
/* Set up the ADC.  Needed for the current sense and pot	*/
void adc_init()
{	
	// Enable the ADC  (ADEN in ADCSRA)
	byte reg = (1<<ADEN) | (1<<ADIE) | (1<<ADIF) | 0x03;
	ADCSRA = reg;  //0x83;

	//	Digital Input Disable (reduce power consumption)
	//  We are using ADC2, ADC8, ADC9 :
	DIDR0  |= (1<<ADC2D);
	DIDR1  |= ((1<<ADC8D) | (1<<ADC9D));

	// HighSpeed Mode & AREF Enable internal circuitry.
	ADCSRB = (1<<ADHSM) | (1<<AREFEN);

	// REFSn bits in ADMUX  (external AREF voltage not connected!)	
	// SELECT THE Channel
	ADMUX = POT_MUX;
}

/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
void start_sampling()
{
	// START ADC CONVERSION:
	ADMUX = POT_MUX;		// Select POT	
	ADCSRA |= (1<<ADSC);	// Start conversion

	// AUTO TRIGGERING MODE (not using)
	// ADCSRA |= ADATE;
}

