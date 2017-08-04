#ifndef _CAN_MSG_H_
#define _CAN_MSG_H_

// Each module sends a message to identify itself (type of device, and 
// SerialNumber)
const long SerialNumber      = 0x56789CDE;

void  	 can_file_message			( sCAN* mMsg 	);
float 	 extract_float_msg			( byte* mdata 	);
word  	 extract_word				( byte* mData 	);
long int extract_long_int			( byte* mData	);
void 	 can_proc_set_stop_msg		  ( sCAN* mMsg 	);
void 	 can_proc_move_to_angle_msg   ( sCAN* mMsg 	);
void 	 can_proc_move_speed_msg	  ( sCAN* mMsg	);
void 	 can_proc_max_acceleration_msg( sCAN* mMsg  );
void 	 can_proc_tilt_msg			  ( sCAN* mMsg 	);

void can_prep_stop		 		( sCAN* mMsg, byte mStopNumber 	 );
void can_prep_motor_angle		( sCAN* mMsg 	);
void can_prep_motor_values_raw	( sCAN* mMsg	);
void can_prep_motor_speed		( sCAN* mMsg, float_array mSpeed );
void can_prep_motor_accel		( sCAN* mMsg, float_array mAccel );
void can_prep_motor_status		( sCAN* mMsg 	);

/******* CAN MESSAGES *****************************************
	RECEIVED
		Mark Stop 1	(Mark the current motor position as Stop 1 - angle given)
		Mark Stop 2	(Mark the current motor position as Stop 2 - angle given)
		Move to Angle 		(Angle, Speed )
		Move to Angle PID   (Angle, Speed depends on distance from Angle )		
**************************************************************/
//void can_proc_set_stop_2_msg( sCAN* mMsg );
#define HARD_STOP_MOTOR 99

void can_proc_set_stop_msg		( sCAN* mMsg );
void can_proc_move_to_angle_msg ( sCAN* mMsg );
void can_proc_move_speed_msg	( sCAN* mMsg );
void can_proc_stop_motor_msg	( sCAN* mMsg );
void can_proc_move_dual_speed_msg( sCAN* mMsg );
//void can_prep_pwm_msg			( sCAN* mMsg, word mPWM );


/*******************************************************
	TRANSMITTED:

**************************************************/
void can_prep_currents( sCAN* mMsg, byte mStopNumber );



#endif
