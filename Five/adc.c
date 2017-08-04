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
#include <avr/io.h>
#include "global.h"

#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "motor.h"
#include "pot.h"
#include "pwm.h"
#include "adc.h"

/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/
const float alpha = 0.2;
const float gamma = (1-alpha);

uint16_t PotSample[5];
uint16_t CurrentSample[5];
float 	 CurrentSampleAvg[5] = { 0.0, 0.0, 0.0, 0.0, 0.0 };

byte next_channel   = 0;
byte sample_counter = 0;
#define NUM_AVGS    10

ISR ( ADC_vect )
{
	if (sample_counter++ > NUM_AVGS) {
		sample_counter=0;
	}
	uint16_t result = ADCL;
	result      += (ADCH<<8);
	int index = 0;
	switch(next_channel)				// Round Robin style measurements
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:	PotSample[next_channel] = result;
			ADMUX = CURRSENSE_LEFT_MUX;		// Select Left Current Read
			ADCSRA |= (1<<ADSC);			// Start conversion
			break;
	
	case 5:
	case 6:		
	case 7:	
	case 8:
	case 9:		
			index = next_channel-5;
			CurrentSample[index]    = result;
			CurrentSampleAvg[index] = (gamma*CurrentSampleAvg[index]) + alpha*((float)CurrentSample[index]);
			ADMUX = CURRSENSE_RIGHT_MUX;	// Select Right Current Read
			ADCSRA |= (1<<ADSC);			// Start conversion
			break;

	default: 			
			next_channel = 0;
			ADMUX   = POT_MUX;		// Select POT	
			ADCSRA |= (1<<ADSC);	// Start conversion
			break;
	}
	
	next_channel++;
	if (next_channel>=9)
		next_channel = 0;
	
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
void init_adc()
{
	ADC_POT_DDR 		   &= ~ADC_POT_MASK;				// inputs =0
	ADC_POT_PORT 		   &= ~ADC_POT_MASK;				// inputs =0
	
	ADC_CURRENT_SENSE_DDR  &= ~ADC_CURRENT_SENSE_MASK;
	ADC_CURRENT_SENSE_PORT &= ~ADC_CURRENT_SENSE_MASK;		// inputs =0

	ADC_CURRENT_SENSE_2_DDR  &=	~ADC_CURRENT_SENSE_2_MASK;
	ADC_CURRENT_SENSE_2_PORT &=	~ADC_CURRENT_SENSE_2_MASK;	// no pullups


	// Enable the ADC  (ADEN in ADCSRA)
	byte reg = (1<<ADEN) | (1<<ADIE) | (1<<ADIF) | 0x03;
	ADCSRA = reg;  //0x83; 

	//	Digital Input Disable (reduce power consumption)
	//  We are using ADC2, ADC8, ADC9 :
	DIDR0  |= (1<<ADC2D);
	DIDR1  |= ((1<<ADC8D) | (1<<ADC9D));

	// HighSpeed Mode & AREF Enable internal circuitry.
//	ADCSRB = (1<<ADHSM) | (1<<AREFEN);

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

float calc_load_current_from_sense_current( uint16_t mIsense )
{
	// ILoad = dK_ilis(Isense - Ioffset)
	return dK * (mIsense - Ioffset);	
}
void convert_to_amps( uint16_t mAmps )
{
	
}
/**********************************************
Return	:	The motor position in degrees * 10
Note: The sense current is based on the highside mosfet
	so we should look to the side which has the upper mosfet
	on to calculate load current.
***********************************************/
uint16_t read_current_left()
{	
	// SELECT AND START CONVERSION:
	ADMUX  = CURRSENSE_LEFT_MUX;
	ADCSRA |= (1<<ADSC);				// Start Conversion

	// WAIT FOR RESULT:
	while ( (ADCSRA & (1<<ADSC)) > 0) {};
	
	// PICK UP RESULT:
	uint16_t result = ADCL;
	result += (ADCH<<8);
	CurrentSample[0] = result;
	return result;
}

/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
uint16_t read_current_right()
{
	// SELECT AND START CONVERSION:
	ADMUX = CURRSENSE_RIGHT_MUX;
	ADCSRA |= (1<<ADSC);				// Start Conversion

	// WAIT FOR RESULT:
	while ( (ADCSRA & (1<<ADSC)) > 0) {};
	
	// PICK UP RESULT:
	uint16_t result = ADCL;
	result      += (ADCH<<8);
	CurrentSample[1] = result;
	return result;
}


