/*************************************************************************
NAME		:	DC_Motor - class to operate enables and direction pins.

				
Author		:  Stephen Tenniswood 2012
Product of Beyond Kinetics, Inc.
*************************************************************************/
/********************************************************
82666 Motor Stats:

No Load motor current    No load  Stall  Model
  Smaller Window Motor  1.2 Amps  10<i<20      032111 
  Passenger Side Motor  1.3 Amps  10<i<20      468702
  Driver Side Motor     1.4 Amps  15<i<20       82666
The stall current was not correctly measured. The power
supply used gave 2.46Amps and then limited it.  Directly
connect to a 12V battery gave meter reading >15a, then
the gears holding it gave way. So still uncertain

Stall Current , Stall Torque
********************************************************/
#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "leds2.h"
#include "motor.h"
#include "DC_Motor.h"


const byte PWM_pin_array[8] = { HBridge_pwm_1_pin, HBridge_pwm_2_pin	};
const byte DIR_pin_array[8] = { HBridge_1_dir_pin, HBridge_2_dir_pin	};


/**********************************************************************//**
*  Purpose	  :  Constructor.  This runs 1 motor.
*  @param 	  :  motor_index [1..8]
*  Return	  :  none
*************************************************************************/
DC_Motor::DC_Motor(byte motor_index) : 
  ForwardDirection( true ),
  Enabled   ( false ),
  MotorIndex( motor_index-1 )
{
	motor_init();
};

/**********************************************************************//**
*  Purpose	  :  Turn Enable line high or low.
*  Parameters :  none
*  Return	  :  none
*************************************************************************/
void  DC_Motor::Enable( bool mEnable )
{
	if (mEnable)
		MotorOn();
	else
		MotorOff();
}
	
/**********************************************************************//**
*  Purpose	  :  Put DC motor in forward direction.  Selects the PWMA pin.
*  Parameters :  none
*  Return	  :  none
*************************************************************************/
void DC_Motor::Forward()
{
	ForwardDirection = true;
	digitalWrite( DIR_pin_array[MotorIndex], HIGH );
}

/**********************************************************************//**
*  Purpose	  :  Put DC motor in backward direction.  Selects the PWMB pin.
*  Parameters :  none
*  Return	  :  none
*************************************************************************/
void DC_Motor::Backward()
{
	ForwardDirection = false;
	digitalWrite(DIR_pin_array[MotorIndex], LOW );
}

/**********************************************************************//**
*  Purpose	  :  Change direction of the motor.
*  Parameters :  none
*  Return	  :  none
*************************************************************************/
void DC_Motor::ToggleDirection()
{
	ForwardDirection = !ForwardDirection;
	if (ForwardDirection)
		Forward();
	else
		Backward();
}

/**********************************************************************//**
*  Purpose	  :  Raise the enable line (PORTC latch).  No effect to PWM duty
*				 or direction.
*  Parameters :  none
*  Return	  :  none
*************************************************************************/
void DC_Motor::MotorOn()
{
	Enabled = true;

	// Use the Latch class constructed with Motor_Clk:
	MotEnables.SetOne( MotorIndex );
	MotEnables.MakeActive();		// make transparent thru the latch
}

/**********************************************************************//**
*  Purpose	  :  Lower the enable line.  No effect to PWM duty or direction.
*  Parameters :  none
*  Return	  :  none
*************************************************************************/
void DC_Motor::MotorOff()
{
	Enabled = false;
	
	// Use the Latch class constructed with Motor_Clk : 
	MotEnables.ClearOne( MotorIndex );
	MotEnables.MakeActive();
}

/**********************************************************************//**
*  Purpose	  :  Set the duty cycle (speed) of the motor.  
*  @param[in] duty	- duty cycle [0..255] Speed of motor.
*  @return	  :  none
*************************************************************************/
void DC_Motor::SetDuty(short duty)
{
	if (duty < 0)
		Backward();

	analogWrite( PWM_pin_array[MotorIndex], duty );
}

