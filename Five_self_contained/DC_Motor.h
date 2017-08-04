/********************************************************
NAME		:	DC Motor
Description	:	Header file for DC Motor class.  

Product of Beyond Kinetics, Inc.
*********************************************************/
#ifndef DC_Motor_h
#define DC_Motor_h


class DC_Motor
{
public:
	DC_Motor(byte motor_index);	// [1..8]
	void  Forward ();			// Selects PWM A (doesn't change the PWM duty)
	void  Backward();			// Selects PWM B
	void  ToggleDirection();	// Switch to other PWM
	bool  getDirection();		// true = forward; false = backward

	void  Enable( bool mEnable );
	void  SetDuty (short duty);	// Sets the duty cycle [0..255]
	void  MotorOn ( );
	void  MotorOff( );	

protected:
	bool ForwardDirection;
	bool Enabled;
	byte MotorIndex;			// [0..7] Constructor subtracts 1.
	
};

#endif
