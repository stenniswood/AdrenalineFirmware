#include <stdlib.h>
#include <math.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
//#include "leds.h"
#include "pwm.h"

#define PRESCALER_1 0x01
#define PRESCALER_8 0x02
#define PRESCALER_64 0x03
#define PRESCALER_256 0x04
#define PRESCALER_1024 0x05
#define PRESCALER_EXTERNAL_FALL 0x06
#define PRESCALER_EXTERNAL_RISE 0x07

//#define PRESCALER_BITS PRESCALER_1		// Fclk
byte    Prescaler = 1;					// Fclk
word    TOP = 0xFFFF;

/* This is proper way to write to the 16 bit register! */
void OCR1A_Write( unsigned int i ) 
{
	unsigned char sreg;	
	sreg = SREG;			/* Save global interrupt flag */ 
	cli();					/* Disable interrupts   */
	OCR1A = i;				/* Set TCNTn to i       */
	SREG  = sreg;			/* Restore global interrupt flag */
}

/* This is proper way to write to the 16 bit register! */
void OCR1B_Write( unsigned int i ) 
{
	unsigned char sreg;
	sreg = SREG;	/* Save global interrupt flag */ 
	cli();			/* Disable interrupts */
	OCR1B = i;		/* Set TCNTn to i     */
	SREG = sreg;	/* Restore global interrupt flag */ 
}

word get_top_value( float mHerz )
{
	// For Prescaler=1;  
	// Not right here:  16,000,000 / 100. = 160,000 which doesn't fit inside a 'word'
	// need prescaler=8;
	//     Right here:   2,000,000 / 100. = 20,000 which does fit inside a 'word'
	word Top = (F_CPU/Prescaler) / mHerz;
	return Top;
}

void set_top( word mTop )
{
	TOP = mTop;
	ICR1 = TOP;
}

word select_prescaler( float mHerz )
{
	word Pre = 1;
	// Calculate the prescaler needed to achieve the mHerz.
	float tick = (16000000.0 / mHerz);
	float val  = tick/(float)Pre;
	if (val>65535) { Pre = 8;    val = tick/(float)Pre; 	}
	if (val>65535) { Pre = 64;   val = tick/(float)Pre; 	}
	if (val>65535) { Pre = 256;  val = tick/(float)Pre; 	}
	if (val>65535) { Pre = 1024; val = tick/(float)Pre; 	}
	return Pre;	
}

byte get_tccr1b_prescaler( word Prescaler )
{
	// Change here will impact BigMotor base frequencies!	
	switch (Prescaler)		// was Prescaler & 0x07
	{
	case 1	  :	return 0x01;	break;
	case 8	  :	return 0x02;	break;
	case 64	  :	return 0x03;	break;
	case 256  :	return 0x04;	break;
	case 1024 :	return 0x05;	break;
	default	  : break;
	}
	return 0xFF;
}

void set_base_PWM_freq( float mHerz )
{
	TOP = get_top_value( mHerz );	
	set_top(TOP);
}

void pwm_init( float mBaseFrequencyHerz, BOOL mA_Enable, BOOL mB_Enable )
{
	if (mA_Enable)		PWM_1A_DDR |= (1<<PWM_1A_PIN);
	if (mB_Enable)		PWM_1B_DDR |= (1<<PWM_1B_PIN);

	// No Power Reduction (needed to enable the Timer1)
	PRR &= ~((1<<PRTIM1) | (1<<PRTIM0));

	// 	16 Bit Registers:
	OCR1A_Write( 0 ); 	// set pwm duty
	OCR1B_Write( 0 ); 	// set pwm duty

	// TOP counter value is defined in the ICR1
	// 16Mhz / 65535 = 	244 Hz  (slowest possible at this prescaler=1)
	set_top( get_top_value( mBaseFrequencyHerz ) );

	// 8 Bit control Registers:
	byte tccr1a = 0x02;
	if (mA_Enable)	tccr1a |= 0x80;
	if (mB_Enable)	tccr1a |= 0x20;
	TCCR1A 	= tccr1a;

	Prescaler   = select_prescaler( mBaseFrequencyHerz );
	byte tccr1b = 0x18;
	TCCR1B = tccr1b | get_tccr1b_prescaler(Prescaler);

	// Mode 14 Fast PWM ICR1 holds TOP
	// We will be using the Waveform Generation Mode (WGM =0000; Normal Mode)
	// Correction!  We will be using FastPWM ICRn is TOP 
	//	(Mode 14)	
}

/*******************************************
set_polarityA(BOOL mHigh)

HIGH (0.33 duty)
  _________
_|		   |_____________________
LOW (0.33 duty)
_		    _____________________
 |_________|
******************************************/
void set_polarityA( BOOL mHigh )
{
	byte tmp = TCCR1A;
	tmp |= 0x80;
	if (mHigh)
		tmp &= ~(0x40);		// Clear PWM pin on compare match
	else
		tmp |= 0x40;		// Set PWM pin on compare match
	TCCR1A = tmp;		
}
void set_polarityB( BOOL mHigh )
{
	byte tmp = TCCR1A;
	tmp |= 0x20;
	if (mHigh)
		tmp &= ~0x10;
	else
		tmp |= 0x10;
	TCCR1A = tmp;
}

/* value must be Positive! 
mDuty	- 	[0 to 1.00]  Fraction
*/
void set_dutyA( float mDuty )
{
	if (mDuty<0)  mDuty = -mDuty;	// make it positive
	word counts = ceil( (float)TOP * mDuty );
	OCR1A_Write( counts );
}

void set_dutyB( float mDuty )
{
	if (mDuty<0) mDuty = -mDuty;
	word counts = ceil( (float)TOP * mDuty );
	OCR1B_Write( counts );
}

