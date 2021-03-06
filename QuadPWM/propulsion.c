/*********************************************************************
Product of Beyond Kinetics, Inc

This code calculates motor speeds to maintain a level quad/octo copter
Based on incoming tilt (combo of gyro/accel doesn't matter here.  we 
receive the tilt and adjust to the desired tilt)

These assume an unknown proportionality constant.  The tilt * constant 
gives ratio of power (left to right side).


DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <avr/sfr_defs.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
#include "can_eid.h"
#include "can_instance.h"
#include "leds.h"
#include "buttons.h"
#include "spi_8500.h"


void calc_left_right()
{
	
}

// Extract the Tilt Sensor for updating PWMS[0..3] 
#define balance_thrust = 0x0200;		// amount needed to recover 90 deg sideways.
#define KConst ((float)1.0)  //(float)0x0200)	
short   thrust = 0x02FF;					// 50% based on 12bit 0x0fff => full scale (pwm)

void can_parse_tilt_msg( sCAN* mMsg )
{
	    led_on(1);
	// Tilts go from -32k to +32k:
	short tilt_x = mMsg->data[1] + (mMsg->data[0]<<8);
	short tilt_y = mMsg->data[3] + (mMsg->data[2]<<8);
	short tilt_z = mMsg->data[5] + (mMsg->data[4]<<8);
	// scale is different here than PWM below, so KConst should be much less than 1

	float left   = ((float)tilt_y * (float)KConst);
	float front  = ((float)tilt_x * (float)KConst);

	short speed1 =  front + left;
	short speed2 =  front - left;
	short speed3 = -front + left;
	short speed4 = -front - left;

	// Update the Quad Motor Speeds:
	PWM_array[M1] = thrust + speed1;
	PWM_array[M2] = thrust + speed2;
	PWM_array[M3] = thrust + speed3;
	PWM_array[M4] = thrust + speed4;
	
	delay(100);
	led_off(1);
}


void can_parse_thrust_msg( sCAN* mMsg )
{
	// We can receive Thrust [0..FFF] as absolute PWM value.
	// OR Z_Accel and adjust thrust until it matches the accel (better method - but units more difficult)	
	thrust       = ((mMsg->data[0] & 0x0F)<<8) + mMsg->data[1];
}










