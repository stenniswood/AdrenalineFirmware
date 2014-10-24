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
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "motor.h"
#include "can_instance.h"
#include "pot.h"
#include "pwm.h"
#include "eeprom.h"
#include "adc.h"

/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/
float LeftCurrentSampleAvg = 0.0;
float RightCurrentSampleAvg = 0.0;
const float alpha = 0.2;
const float gamma = (1-alpha);

word LeftCurrentSample;
word RightCurrentSample;
word PotSample[2];
byte next_channel = 0;
byte sample_counter = 0;
#define NUM_AVGS 10

ISR ( ADC_vect )
{
	if (sample_counter++ > NUM_AVGS) {
		sample_counter=0;
	}
	word result = ADCL;
	result      += (ADCH<<8);
	switch(next_channel)				// Round Robin style measurements
	{
	case 0:	PotSample[1] = result;
			next_channel = 1;
			ADMUX = CURRSENSE_LEFT_MUX;		// Select Left Current Read
			ADCSRA |= (1<<ADSC);			// Start conversion
			break;
			
	case 1:	LeftCurrentSample = result;
			LeftCurrentSampleAvg = (gamma*LeftCurrentSampleAvg) + alpha*((float)LeftCurrentSample);
			next_channel = 2;
			ADMUX = CURRSENSE_RIGHT_MUX;	// Select Right Current Read
			ADCSRA |= (1<<ADSC);			// Start conversion
			break;
			
	case 2:	RightCurrentSample = result;	
			RightCurrentSampleAvg = (gamma*RightCurrentSampleAvg) + alpha*((float)RightCurrentSample);
			next_channel = 0;
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
float dK      = 1.0;
float Ioffset = 0.0;

float calc_load_current_from_sense_current( word mIsense )
{
	// ILoad = dK_ilis(Isense - Ioffset)
	return dK * (mIsense - Ioffset);	
}
void convert_to_amps( word mAmps )
{
	
}
/**********************************************
Return	:	The motor position in degrees * 10
Note: The sense current is based on the highside mosfet
	so we should look to the side which has the upper mosfet
	on to calculate load current.
***********************************************/
word read_current_left()
{	
	// SELECT AND START CONVERSION:
	ADMUX  = CURRSENSE_LEFT_MUX;
	ADCSRA |= (1<<ADSC);				// Start Conversion

	// WAIT FOR RESULT:
	while ( (ADCSRA & (1<<ADSC)) > 0) {};
	
	// PICK UP RESULT:
	word result = ADCL;
	result += (ADCH<<8);
	LeftCurrentSample = result;
	return result;
}
/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
word read_current_right()
{
	// SELECT AND START CONVERSION:
	ADMUX = CURRSENSE_RIGHT_MUX;
	ADCSRA |= (1<<ADSC);				// Start Conversion

	// WAIT FOR RESULT:
	while ( (ADCSRA & (1<<ADSC)) > 0) {};
	
	// PICK UP RESULT:
	word result = ADCL;
	result      += (ADCH<<8);
	RightCurrentSample = result;		
	return result;
}


