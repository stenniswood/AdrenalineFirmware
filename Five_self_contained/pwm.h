#ifndef _PWM_H_
#define _PWM_H_

#include "global.h"

//extern float MotDuties[4];
void stop_motors();
void resume_motors();


extern uint16_t TOP;


// For atmega16m1 micro, the 16 bit output compares will always have the same DDR/PORT
// Configurations.  So we don't need to define these in pin_definitions.

#define PWM_1A_DDR DDRB
#define PWM_1B_DDR DDRB
#define PWM_1C_DDR DDRB
#define PWM_4A_DDR DDRH
#define PWM_4B_DDR DDRH

#define PWM_1A_PORT PORTB
#define PWM_1B_PORT PORTB
#define PWM_1C_PORT PORTB
#define PWM_4A_PORT PORTH
#define PWM_4B_PORT PORTH

#define PWM_1A_PIN 5		// PB5
#define PWM_1B_PIN 6		// PB6
#define PWM_1C_PIN 7		// PB7
#define PWM_4A_PIN 3		// PH3
#define PWM_4B_PIN 4		// PH4


void OCR1A_Write  		( unsigned int i );
void OCR1B_Write  		( unsigned int i );
void OCR1C_Write  		( unsigned int i );
void OCR4A_Write		( unsigned int i );
void OCR4B_Write		( unsigned int i );


uint16_t select_prescaler    ( float mHerz	  );
uint8_t  get_tccr1b_prescaler( uint16_t Prescaler );

uint16_t get_top_value		( float mHerz    );
void     set_top	  		( uint16_t mTop 	 );

float get_base_frequency_herz( );
void  set_base_PWM_freq	( float mHerz 	 );


void set_duty_M1		( float mDuty );
void set_duty_M2		( float mDuty );
void set_duty_M3		( float mDuty );
void set_duty_M4		( float mDuty );
void set_duty_M5		( float mDuty );
void set_motor_duty		( int mIndex, float mFraction );


void set_polarityA		( BOOL mHigh );
void set_polarityB		( BOOL mHigh );

void pwm_init	  		( float mBaseFrequencyHerz, BOOL mEnable );


#endif
