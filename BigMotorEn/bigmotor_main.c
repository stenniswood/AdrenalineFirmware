/*********************************************************************
Sends a CAN message with the info

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
	CAN messaging:		
	Encoders	
Configuration
Byte 1:	0x08	=> Free Spin, Slowest base frequency
Byte 2:	
Byte 3:	
Byte 4:	
-----------------------------------
Firmup the software / Glitch List:
		a) Test for full power	
		b) POT (stops) when free spin configured.
********************************************************************/
#include "iom16m1.h"
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "inttypes.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "leds.h"
#include "../core/can.h"
#include "can_msg.h"
#include "can_board_msg.h"
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

void can_prep_eeprom_dump_msg( sCAN* mMsg, byte* addr )
{
	mMsg->id  = create_CAN_eid( 0x0111, MyInstance );
    mMsg->data[0] = (eeprom_read_byte( addr++ ));
    mMsg->data[1] = (eeprom_read_byte( addr++ ));
    mMsg->data[2] = (eeprom_read_byte( addr++ ));
    mMsg->data[3] = (eeprom_read_byte( addr++ ));
    mMsg->data[4] = (eeprom_read_byte( addr++ ));
    mMsg->data[5] = (eeprom_read_byte( addr++ ));
    mMsg->data[6] = (eeprom_read_byte( addr++ ));
    mMsg->data[7] = (eeprom_read_byte( addr++ ));
    mMsg->header.DLC	= 8;
    mMsg->header.rtr    = 0;
}
void can_prep_eeprom_dump1_msg( )
{
	byte* addr = INSTANCE_EEPROM_ADDRESS;
	can_prep_eeprom_dump_msg( &msg1, addr );
	can_send_msg( 0, &msg1 );
}
void can_prep_eeprom_dump2_msg( )
{
	byte* addr = (byte*)(INSTANCE_EEPROM_ADDRESS+8);
	can_prep_eeprom_dump_msg( &msg2, addr );
	can_send_msg( 2, &msg2 );
}
void can_prep_eeprom_dump3_msg( )
{
	byte* addr = (byte*)(INSTANCE_EEPROM_ADDRESS+16);
	can_prep_eeprom_dump_msg( &msg1, addr );
	can_send_msg( 4, &msg1 );
}
byte ResetReason = 0x00;

void init()
{
	ResetReason = MCUSR;		
 	cli();
    chip_init ();    				// Chip initialization
	init_leds ();
	delay(1000000);					// ~ 2 sec
	read_cal();						// Read everything including motor stops.	
	// yes can_init() needs MyInstance to be set already for filtering!
	can_init(CAN_250K_BAUD);		/* Enables Mob0 for Reception! */

    // INIT MYINSTANCE:	
	config_init();
	can_instance_init();
	
    set_rx_callback			( can_file_message );
	set_configure_callback	( config_change    );
	sei();

	OS_InitTask();
	pot_init();	
	motor_init ();
	encoder_init();
	
//	can_prep_instance_request( &msg2, 0xBB );
//	can_send_msg( 0, &msg2 );
}

//********** main *****************************************************
word prevPosition = 0;
int main(void)
{
	init();
    while (1)
    {	
		//motor_set_duty( 0.75 );		

    	// CAN Messaging will directly Start/Stop the motor.
//   	SET_LED_3();
//		delay(one_second); delay(one_second); 
//		RESET_LED_3();
		delay(one_second); delay(one_second); 

//		can_prep_eeprom_dump1_msg( );
//		can_prep_eeprom_dump2_msg( );
//		can_prep_eeprom_dump3_msg( );			
    }
    return (0);
}

