#ifndef _ADC_H_
#define _ADC_H_


#define MAX_USED_CHANNELS 8
#define NUM_AVGS    	  10

extern float SampleAvg   [MAX_USED_CHANNELS];		// Samples averaged 
extern word  Sample      [MAX_USED_CHANNELS];		// Raw samples

#define FIRST_CHANNEL_MUX	0x41

// Used in Big Motor to read the POT, Motor Current 1 & 2
void random_number_adc_init();		// Special init used by can_instance if adc_init(); has not been called.
void adc_init();
word read_current_right();
word read_current_left();
void start_sampling();
void adc_sleep();


#endif
