/*********************************************************************
Sends a CAN message with the info

To compile on MacBook Pro:
Add : /usr/local/CrossPack-AVR-20120217/bin:
to the front of PATH environment variable.  For example:
> PATH=/usr/local/CrossPack-AVR-20120217/bin:$PATH

AFTER COMPILE, PROGRAM WITH:
avrdude -p atmega32m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uflash:w:./bigmotor.hex
avrdude -p atmega32m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Ulfuse:w:0xE4:m
avrdude -p atmega32m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uefuse:w:0xFC:m

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
	Encoders	
Configuration
Byte 1:	0x08	=> Free Spin, Slowest base frequency
Byte 2:	
Byte 3:	
Byte 4:	
********************************************************************/
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include <avr/io.h>

#include "inttypes.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "leds2.h"
#include "motor.h"
#include "pot.h"
#include "encoder.h"
#include "pwm.h"
#include "OS_Dispatch.h"
#include "OS_timers.h"
#include "adc.h"
#include "buttons.h"
#include "limit_switches.h"
#include "protocol.h"
#include "serial.h"
#include "configuration.h"


bool okay_to_read_cal = false;
byte ResetReason = 0x00;

void init()
{
	ResetReason = MCUSR;		
 	cli();
    //chip_init ();    				// Chip initialization
    
	init_leds ();
	init_buttons();	
	init_limit_switches();
	init_motor_control_lines();
	
	float mBaseFrequencyHerz = 500.0;  // 4khz
	pwm_init( mBaseFrequencyHerz, TRUE  );
	init_serial();
	
	init_configuration();
	read_configuration();
	
	//encoder_init();
	//pot_init   ();
			
	delay(100000);					// ~ 2 sec
	//read_cal();					// Read everything including motor stops.
	sei();

	//OS_InitTask();
}

//********** main *****************************************************
word prevPosition = 0;

int main(void)
{
	init();
set_duty_M1( 0.25 );
set_duty_M2( 0.5 );
set_duty_M3( 0.75 );
set_duty_M4( 0.95 );
bool motors_stopped = true;

    while (1)
    {	
#define STOP_LED 1

    	// CAN Messaging will directly Start/Stop the motor.
		delay(one_second/20);
		led_on( 4 );
		led_on( 2 );
		led_on( 3 );
		delay(one_second/20);
		led_off( 4 );
		led_off( 2 );
		led_off( 3 );
		if (motors_stopped)
			led_on ( STOP_LED );
		else
			led_off( STOP_LED );

		read_buttons();
		if (stop_button_pressed)
		{
			motors_stopped = true;
			stop_motors();
		}
		if (resume_button_pressed)
		{
			motors_stopped = false; 
			resume_motors();
		}

		read_limit_switches();
		if (is_any_limit_triggered())
			motors_stopped = TRUE;	// indication only here.

		motor_timeslice_10ms();		
		// test message
		send_positions();		
    }
    return (0);
}

