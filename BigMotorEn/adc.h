#ifndef _ADC_H_
#define _ADC_H_


extern word PotSample[2];

extern word LeftCurrentSample ;
extern word RightCurrentSample;

extern float LeftCurrentSampleAvg;
extern float RightCurrentSampleAvg;

// Selects signal for ADC conversion:
#define POT_MUX				0x42	// PD5  ADC2
#define CURRSENSE_LEFT_MUX  0x48	// PC4	ADC8
#define CURRSENSE_RIGHT_MUX 0x49	// PC5	ADC9

// Used in Big Motor to read the POT, Motor Current 1 & 2
void adc_init			();
word read_current_right	();
word read_current_left	();
void start_sampling		();
void adc_sleep			();




#endif
