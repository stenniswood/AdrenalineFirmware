#ifndef _ADC_H_
#define _ADC_H_

#include "global.h"


extern uint16_t PotSample[5];
//extern short PotSpeed[5];
//extern short PotAcceleration[5];

extern uint16_t CurrentSample[5];
extern float 	CurrentSampleAvg[5];


/* Group 1 */
#define ADC_POT_DDR  	DDRA
#define ADC_POT_PORT 	PORTA
#define ADC_POT_PIN 	PINA
#define ADC_POT_MASK 	0x1F
#define ADC_CURRENT_SENSE_DDR  	DDRA
#define ADC_CURRENT_SENSE_PORT 	PORTA
#define ADC_CURRENT_SENSE_PIN 	PINA
#define ADC_CURRENT_SENSE_MASK 	0xE0

/* Group 2 */
#define ADC_CURRENT_SENSE_2_DDR  	DDRC
#define ADC_CURRENT_SENSE_2_PORT 	PORTC
#define ADC_CURRENT_SENSE_2_PIN 	PINC
#define ADC_CURRENT_SENSE_2_MASK 	0x03


// Selects signal for ADC conversion:
#define POT_MUX				0x42	// PD5  ADC2
#define CURRSENSE_LEFT_MUX  0x48	// PC4	ADC8
#define CURRSENSE_RIGHT_MUX 0x49	// PC5	ADC9


// Used in Big Motor to read the POT, Motor Current 1 & 2
void 	 init_adc			();
uint16_t read_current_right	();
uint16_t read_current_left	();
void 	 start_sampling		();
void 	 adc_sleep			();




#endif
