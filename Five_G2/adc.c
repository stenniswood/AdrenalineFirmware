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
#include "motor.h"
#include "pwm.h"
#include "adc.h"

/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/
//const float alpha = 0.2;
//const float gamma = (1-alpha);
//float 		dK      = 1.0;
//float 		Ioffset = 0.0;

uint16_t PotSample[5];
uint16_t CurrentSample[5];
float 	 CurrentAmps[5] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
BOOL     Done_sampling = FALSE;

byte 	current_index  = 0;
byte 	pot_index	   = 0;
byte 	next_channel   = 0;
byte 	sample_counter = 0;
#define NUM_AVGS    10


ISR ( ADC_vect )
{
/*	if (sample_counter++ > NUM_AVGS) {
		sample_counter=0;
	} */
	// GET ADC RESULT:
	uint16_t result = ADCL;
	result      += (ADCH<<8);
	
	switch(next_channel++)				// Round Robin style measurements
	{
	/* Read 4 POTs */
	case 0: pot_index = 0;
			Done_sampling = FALSE;
			PotSample[pot_index++] = result;	 
			ADMUX = 0x41;	ADCSRA |= (1<<ADSC);			// Start conversion
			break;	
	case 1:	PotSample[pot_index++] = result;	 
			ADMUX = 0x42;	ADCSRA |= (1<<ADSC);			// Start conversion	
			break;	
	case 2:	PotSample[pot_index++] = result;	
			ADMUX = 0x43;	ADCSRA |= (1<<ADSC);			// Start conversion	
			break;	
	case 3: PotSample[pot_index++] = result;				// setup current sense 1
			select_currents();
			break;

	/* Current Sense 1..5, POT_5 (ADC8..ADC12,ADC13)  */
	case 4:	current_index = 0;
			CurrentSample[current_index++]    = result;
			ADMUX = 0x41;	ADCSRA |= (1<<ADSC);			// setup sense 2, Start conversion	
			break;
	case 5:  CurrentSample[current_index++]    = result;
			ADMUX = 0x42;	ADCSRA |= (1<<ADSC);			// 3 Start conversion	
			break;
	case 6:	 CurrentSample[current_index++]    = result;
			ADMUX = 0x43;	ADCSRA |= (1<<ADSC);			// 4 Start conversion	
			break;
	case 7:	 CurrentSample[current_index++]    = result;
			ADMUX = 0x44;	ADCSRA |= (1<<ADSC);			// 5 Start conversion	
			break;
	case 8:	 CurrentSample[current_index++]    = result;
			ADMUX = 0x45;	ADCSRA |= (1<<ADSC);			// 6 Start conversion	
			break;
	case 9:	PotSample[pot_index++] = result;	// POT 5	
			next_channel = 0;	
			Done_sampling = TRUE;			
//			select_pots();
			break;			
	default: 			
			next_channel = 0;
			Done_sampling = TRUE;
			//select_pots();
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
void init_adc()
{
	ADC_POT_DDR 		   &= ~ADC_POT_MASK;				// inputs =0
	ADC_POT_PORT 		   &= ~ADC_POT_MASK;				// inputs =0

	ADC_POT_2_DDR 		   &= ~ADC_POT_2_MASK;				// inputs =0
	ADC_POT_2_PORT 		   &= ~ADC_POT_2_MASK;				// inputs =0
	
	// Enable the ADC  (ADEN in ADCSRA)
	byte reg = (1<<ADEN) | (1<<ADIE) | (1<<ADIF) | 0x03;
	ADCSRA = reg;  //0x83;

	//	Digital Input Disable (reduce power consumption)
	//  We are using ADC2, ADC8, ADC9 :
	DIDR0  |= 0x0F;
	DIDR1  |= 0x3F;

	// HighSpeed Mode & AREF Enable internal circuitry.
//	ADCSRB = (1<<ADHSM) | (1<<AREFEN);

	// REFSn bits in ADMUX  (external AREF voltage not connected!)		
	start_sampling();
}

/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
void start_sampling()
{
	select_pots();
}


float calc_load_current_from_sense_voltage( uint16_t mIsenseV )
{
	const float K_ilis   = 8500;		// kILIS = IL / IIS	=8500
	const float resistor = 1000;

	float currentSense = mIsenseV / (resistor);
	float current      = K_ilis * currentSense;
	return current;

	// ILoad = dK_ilis(Isense - Ioffset)
	//return dK * (mIsense - Ioffset);	
}

void adc_timeslice()
{
	for (int m=0; m<NUM_MOTORS; m++)
		CurrentAmps[m] = calc_load_current_from_sense_voltage( CurrentSample[m] );
}

/**********************************************
Return	:	The motor position in degrees * 10
Note: The sense current is based on the highside mosfet
	so we should look to the side which has the upper mosfet
	on to calculate load current.
***********************************************/
void select_currents()
{	
	// SELECT AND START CONVERSION:
	ADMUX  = 0x40;
	ADCSRB |= 0x08;					// set mux bit 5.	
	ADCSRA |= (1<<ADSC);			// Start Conversion
}

void select_pots()
{	
	// SELECT AND START CONVERSION:
	ADMUX = 0x40;
	ADCSRB &= ~0x08;				// reset mux bit 5.	
	ADCSRA |= (1<<ADSC);			// Start Conversion
}

