/*********************************************************************
Sends a CAN message with the info

AFTER COMPILE, PROGRAM WITH:
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uflash:w:./bigmotor.hex

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
Product of Beyond Kinetics, Inc   

============================== CAN MESSAGES ===========================
// RECEIVING MESSAGE IDs (higher ids should prioritize higher):
#define ID_MARK_MOTOR_STOP				0x0040		// 1 or 2 in data[0]
#define ID_MOVE_TO_ANGLE				0x0042		// Instance goes with intended Receiver
#define ID_MOVE_SPEED					0x0043
#define ID_SET_MAX_MOTOR_ACCEL			0x0044

// TRANSMITTING MESSAGE IDs:
#define ID_CALIBRATED_STOP				0x0045		// Upon request FOR DEBUG  1 or 2 in data[0]
#define ID_MOTOR_ANGLE					0x0046		// Instance goes with Transmitter
#define ID_MOTOR_VALUE					0x0047
#define ID_MOTOR_SPEED					0x0048
#define ID_MOTOR_ACCEL					0x0049
#define ID_MOTOR_STATUS					0x004A
=====================================================================
Strategy:
	Receive CAN Messages:
		Mark_Stop_1,Mark_Stop_2 (receives an angle float and a position - if position is -1; 
								 mark current motor position)
			Data will be saved into EEPROM

		Generate 2 PWMs for motor.
			Motors can be run from -100.0 to +100.0 percent.
=====================================================================
TODO:
	Motor Currents threshold saved to EEPROM.
	CAN messaging:
		
	Encoders	
	Mode/Configuration byte (from CAN, stored in EEPROM)
		toggles between POT & encoder useage.	
		Frequency to send current & motor angle (off to every 20ms; 8 values)
		
********************************************************************/
#include "iom16m1.h"
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "inttypes.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "../core/leds.h"
#include "../core/can.h"
#include "can_msg.h"
#include "buttons.h"
#include "motor.h"
#include "pot.h"
#include "encoder.h"
#include "eeprom.h"
#include "pwm.h"
#include "OS_Dispatch.h"
#include "OS_timers.h"
#include "adc.h"
#include "configuration.h"
#include "calibrations.h"


/*	Firmup the software.  
	Glitch List:
		a) Test for full power	
		b) POT (stops) when free spin configured.
		c) 		

*/
void init()
{
    chip_init ();    				// Chip initialization
	init_leds ();
	can_init  (CAN_250K_BAUD);		/* Enables Mob0 for Reception! */
    can_remove_instance_from_filter( 1 );
    // INIT MYINSTANCE:
	MyInstance = 0;
	random_number_adc_init();
	can_instance_init();
    set_rx_callback(can_file_message);
	start_sampling();
	
	OS_InitTask();	
	motor_init ();
	//motor_set_duty(1.0);
}
//********** main *****************************************************
word prevPosition = 0;
int main(void)
{
	init();
	//set_top( 0x3FF );
/*	can_prep_stop( &msg1, 0 );
	can_send_msg ( 0, &msg1 );
	can_prep_stop( &msg1, 1 );
	can_send_msg ( 0, &msg1 ); */
	
    while (1)
    {	
    	// CAN Messaging will directly Start/Stop the motor.
		delay(one_second/6);		

		//float percent = pot_percent();
		//motor_set_duty( percent );

/*		motor_set_duty( duty );
		duty += delta;
		if (duty>0.9) {
			delta = -delta;
			//motor_enable(FALSE);
		} else if (duty<-0.9)
			delta = -delta;  */
    }
    return (0);
}

/* To summarize BigMotor Board :
		PWM for motors required a certain order of registers in PWM_init
		Now working.

	float delta = 0.1;
	float duty = -1.0;

	BOOL  Up   = TRUE;
	float duty = 99.0;
	float delta = 1.0;
		if (Up)		duty += delta;
		else 		duty -= delta;
		if (duty<delta)   	  Up = TRUE;
		if (duty>(1-delta))   Up = FALSE;					
*/