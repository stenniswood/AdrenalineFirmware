#ifndef _ADC_H_
#define _ADC_H_

#include "global.h"


extern uint16_t PotSample[5];
//extern short PotSpeed[5];
//extern short PotAcceleration[5];

extern uint16_t CurrentSample[5];
extern float 	CurrentSampleAvg[5];


/* Group 1 */
#define ADC_POT_DDR  	DDRF
#define ADC_POT_PORT 	PORTF
#define ADC_POT_PIN 	PINF
#define ADC_POT_MASK 	0x0F

/* Group 2 */
#define ADC_POT_2_DDR  	DDRK
#define ADC_POT_2_PORT 	PORTK
#define ADC_POT_2_PIN 	PINK
#define ADC_POT_2_MASK 	0x3F

/* Group 1 
#define ADC_CURRENT_SENSE_DDR  	DDRK
#define ADC_CURRENT_SENSE_PORT 	PORTK
#define ADC_CURRENT_SENSE_PIN 	PINK
#define ADC_CURRENT_SENSE_MASK 	0x1F*/


// Selects signal for ADC conversion:
//#define POT_MUX				0x00	// PD5  ADC2
//#define CURRSENSE_MUX  0x00	// PC4	ADC8   set bit 5 in ADCSRB


// Used in Big Motor to read the POT, Motor Current 1 & 2
void 	 init_adc			();
void	 adc_timeslice		();

uint16_t read_current_right	();
uint16_t read_current_left	();
void 	 start_sampling		();
void 	 adc_sleep			();

extern BOOL     Done_sampling;

void select_currents();
void select_pots    ();



#endif
