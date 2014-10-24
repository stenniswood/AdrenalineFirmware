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
#include "pwm.h"
#include "eeprom.h"
//#include "adc.h"
#include "analog.h"

/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/

float SampleAvg   [MAX_USED_CHANNELS];		// 8 Pins
float accumulators[MAX_USED_CHANNELS];
word  Sample      [MAX_USED_CHANNELS];

byte sample_index   = 0;
byte next_channel   = 1;
byte sample_counter = 0;

ISR ( ADC_vect )
{
	if (sample_counter++ > NUM_AVGS) 
	{
		sample_counter=0;		
		for (int i=0; i<MAX_USED_CHANNELS; i++)
		{
			// maybe best not to do in interrupt!
			//SampleAvg[i] = accumulators[i] / NUM_AVGS;		// could be expensive time wise!
			accumulators[i] = 0.0;		// start over
		}
	} 
	word result = ADCL;
	result      += (ADCH<<8);

	next_channel++;
	if (next_channel==4) 
		next_channel++;
	if (next_channel==MAX_USED_CHANNELS)
		next_channel = 1;  

	Sample      [sample_index] = result;
	//accumulators[sample_index] += result;  This caused serious errors!
	sample_index++;
	if (sample_index == MAX_USED_CHANNELS)
	{
		sample_index = 0;
		next_channel = 1;
	} 

	byte admux = (ADMUX & 0xE0);
	ADMUX  = (admux | next_channel);
	ADCSRA |= (1<<ADSC);				// Start conversion

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

/*****************************************
 MAPPINGS:
ADC PIN 	Atmel ADC_x		PORT
1			ADC1			PD4
2			ADC2			PD5
3			ADC3			PD6
4			ADC5*			PB2
5			ADC6			PB5
6			ADC7			PB6
7			ADC9			PC5
8			ADC8			PC4
*****************************************/

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
	//  We are using ADC0..ADC7 
	// ADC4 is PB7 which is used as SCLK.  So has been rewired to 
	// 
	DIDR0  = 0xEF;		// ADC4=0 FOR SCLK
	DIDR1  = 0X03;		// ADC8,ADC9

	// HighSpeed Mode & AREF Enable internal circuitry.
	ADCSRB = (1<<ADHSM) | (1<<AREFEN);

	// REFSn bits in ADMUX  (external AREF voltage not connected!)	
	// SELECT THE Channel
	ADMUX = FIRST_CHANNEL_MUX;
}

/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
void start_sampling()
{
	// START ADC CONVERSION:
	ADMUX = FIRST_CHANNEL_MUX;		// Select POT	
	ADCSRA |= (1<<ADSC);			// Start conversion

	// AUTO TRIGGERING MODE (not using)
	// ADCSRA |= ADATE;
}


