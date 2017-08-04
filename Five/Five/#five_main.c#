/*********************************************************************
Sends a CAN message with the info

To compile on MacBook Pro:
Add : /usr/local/CrossPack-AVR-20120217/bin:
to the front of PATH environment variable.  For example:
> PATH=/usr/local/CrossPack-AVR-20120217/bin:$PATH
PATH=/usr/local/CrossPack-AVR-20131216/bin:$PATH
PATH=/usr/local/gcc-6.1.0/:$PATH

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
=====================================================================
TODO:
	Motor Currents threshold saved to EEPROM.
	Encoders	
Configuration
********************************************************************/
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/wdt.h>

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
#include "global.h"

/* Watchdog timer Routines:  */
#define watchdog_reset() __asm__ __volatile__ ("wdr")

void WDT_off()
{
  //__disable_interrupt();
  cli();
  watchdog_reset();
  
  /* Clear WDRF in MCUSR */
  MCUSR &= ~(1<<WDRF);
  /* Write logical one to WDCE and WDE */
  /* Keep old prescaler setting to prevent unintentional time-out
*/
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  /* Turn off WDT */
  WDTCSR = 0x00;
  //__enable_interrupt();
  	sei();
}

void WDT_Prescaler_Change(void)
{
  //__disable_interrupt();
   	cli();
  watchdog_reset();
  
  /* Start timed  equence */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  // PREDEFINED TIMEOUT HERE!!!...  
  /* Set new prescaler(time-out) value = 64K cycles (~0.5 s) */
  WDTCSR  = (1<<WDE) | (1<<WDP2) | (1<<WDP0);
  //__enable_interrupt();
  sei();
}

BOOL okay_to_read_cal = FALSE;
byte ResetReason = 0x00;
char* get_reset_reason_string()
{
	static char buff[50];
	strcpy(buff, "Reason:");
	if (ResetReason & 0x01) strcat( buff, "Power On Reset ");
	if (ResetReason & 0x02) strcat( buff, "External Reset ");
	if (ResetReason & 0x04) strcat( buff, "Brown Out Reset ");
	if (ResetReason & 0x08) strcat( buff, "Watchdog Reset ");	
	if (ResetReason & 0x10) strcat( buff, "JTAG Reset ");
	MCUSR = 0x00;
	return buff;
}

void init()
{
	ResetReason = MCUSR;	
		
 	cli();
    WDT_off();		// Turn off until initialization is done and we can pet the dog.    
    
	init_leds ();
	init_buttons();	
	init_limit_switches();
	init_motor();
	
	float mBaseFrequencyHerz = 500.0;  // 4khz
	pwm_init( mBaseFrequencyHerz, TRUE  );

	init_serial();	
	init_configuration();
//	read_configuration_and_set();
//	init_adc();				
//	start_sampling();	
	
	encoder_init();
//	pot_init   ();
			
	
	WDT_Prescaler_Change();
	
	//delay(100000);					// ~ 2 sec
	//read_cal();					// Read everything including motor stops.
	sei();

	//OS_InitTask();
}

//********** main *****************************************************
word prevPosition = 0;
void send_welcome()
{
	char buffer[100];	
	sprintf(buffer,"WELCOME - BEYOND KINETICS - DRIVE FIVE :\r\n");
	strcat (buffer, get_reset_reason_string() );
	strcat (buffer,"\r\nType 'help' or 'list' for command list.\r\n>");
	send_message( buffer );
//	send_prompt();
}

void mot_test()
{
	set_duty_v( 0.25 );
	set_duty_w( 0.5 );
	set_duty_x( 0.75 );
	set_duty_y( 0.95 );
	set_duty_z( 1.00 );
}

int main(void)
{	
	init();
	BOOL motors_stopped = FALSE;
	
	send_welcome();
	        
    while (1)
    {	
		if (motors_stopped)
		{
			led_on ( STOP_LED );
			led_off ( GO_LED );
		} else
		{
			led_on( GO_LED );
			led_off( STOP_LED );
		}
		
		read_buttons();
		if (stop_button_pressed)
		{
			motors_stopped = TRUE;
			stop_motors();
		}
		if (prog_button_pressed)
		{
			motors_stopped = FALSE; 
			resume_motors();
		}

		encoder_timeslice();		
		limit_switch_timeslice();
		motor_timeslice_10ms();		
		serial_timeslice();		// handle incoming messages (if avail)

		watchdog_reset();
		//delay(one_second/10);
    }
    return (0);
}

