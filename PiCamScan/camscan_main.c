/*********************************************************************
Product of Beyond Kinetics, Inc

This code reads the keypad array of buttons
and sends a CAN message with the info

INCOMING MESSAGES:
	ID_SYSTEM_LED_REQUEST
	ID_SYSTEM_LED_RELEASE

OUTGOING MESSAGES:
	ID_BUTTON_ROSTER

AFTER COMPILE, PROGRAM WITH:
avrdude -p atmega32m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uflash:w:./PiCamScan.hex
avrdude -p atmega32m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Ulfuse:w:0xE4:m

Main Features:
	SPI slave - receives from Raspberry Pi - Hope the 3.3v is not a problem!!
	Read 8 Analog inputs.
	Set Lowside drivers based on msgs in SPI communications.
	Respond to CAN msgs
	Adjust Servo PWMs

Test App:
	Set Servos to scan automatically.
	
TODO    : 

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <math.h>
#include "bk_system_defs.h"
#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "../core/leds.h"
#include "can.h"
#include "can_msg.h"
#include "buttons.h"
#include "OS_timers.h"
#include "can_id_list.h"
#include "can_eid.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "configuration.h"
#include "calibrations.h"
#include "lowside.h"
#include "servos.h"
#include "analog.h"
#include "spi.h"
#include "rpi_spi.h"
#include "pwm.h"

#define CAN_TRANSMIT_CHANNEL1 0
#define CAN_TRANSMIT_CHANNEL2 2
struct sCAN keydata;

/*void adc_init()
{
	DIDR0 = 0;
	DIDR1 = 0;
	ADCSRA = 0;	// disabled!
}*/

void init()
{
	cli();
	chip_init   ( );	
	// CAM SCAN's LEDs go directly from the Raspberry Pi and not this Atmel part.
	
	delay(10000);					// ~ 1 sec
	//read_cal();
	//can_init( CAN_250K_BAUD );			/* Enables Mob0 for Reception! */

	//config_init();
	//can_instance_init();	
	//set_configure_callback	( config_change );			
	//set_rx_callback( can_file_message );	// empty

	spi_init( FALSE, FALSE, 0, 0 );
	set_spi_isr_callback( spi_callback );
	sei();

//	lowside_init  ( );
	servos_init   ( );
//	adc_init      ( );
//	start_sampling( );
	
	OS_InitTask   ( );
}


//***** main *****************************************************
byte  count = 0;
float angle = -60.0;
float increment = (60.0*2.0 / 600.0);
void servo_scan()
{
	/* Scan both servos!		*/
	if (angle >  60.0) increment = -fabs(increment);
	if (angle < -60.0) increment = +fabs(increment);
	angle  += increment;

	set_angle(1, angle);
	set_angle(2, angle);
}

void process_spi_msg()
{
	
}

int main(void)
{
    init();
    set_angle(1, 0.0);	    
    
    while (1)
    {
	    //set_angle(2, -30.0);
		//servo_scan();
		//delay(10000);
		//set_angle(2, 30.0);
		delay(10000);		
		process_spi_msg();
    }
    return(0);
} 

