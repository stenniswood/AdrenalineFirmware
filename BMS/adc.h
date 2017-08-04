#ifndef _ADC_H_
#define _ADC_H_



extern word CurrentSamples[4];
extern float CurrentSampleAvg[4];
extern float Cumulative[4];


// Selects signal for ADC conversion:

#define CURRSENSE_1_MUX  0x47	// PC4	ADC6
#define CURRSENSE_2_MUX  0x47	// PC4	ADC7
#define CURRSENSE_3_MUX  0x48	// PC4	ADC8
#define CURRSENSE_4_MUX 0x49	// PC5	ADC9

// Used in BMS to read the currents on each of 4 5v supplies.
void adc_init			();
void start_sampling		();
void adc_sleep			();


#endif
