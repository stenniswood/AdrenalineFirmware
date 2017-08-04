#ifndef _ADC_H_
#define _ADC_H_

#include "global.h"


extern uint16_t PotSample[4];
//extern short PotSpeed[2];
//extern short PotAcceleration[2];

extern uint16_t LeftCurrentSample ;
extern uint16_t RightCurrentSample;

extern float LeftCurrentSampleAvg;
extern float RightCurrentSampleAvg;

// Selects signal for ADC conversion:
#define POT_MUX				0x42	// PD5  ADC2
#define CURRSENSE_LEFT_MUX  0x48	// PC4	ADC8
#define CURRSENSE_RIGHT_MUX 0x49	// PC5	ADC9

// Used in Big Motor to read the POT, Motor Current 1 & 2
void adc_init			();
uint16_t read_current_right	();
uint16_t read_current_left	();
void start_sampling		();
void adc_sleep			();




#endif
