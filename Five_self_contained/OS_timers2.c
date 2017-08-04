/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Low Level Routines RTOS					 	 	 *
 *											  						 *
 *	  CATEGORY:   BIOS Driver										 *
 *											   						 *
 *    PURPOSE: Use Timer 0 for a 1ms tic.  Interrupt handler to 	 *
 *           set update counters, then dispatch any tasks.			 *
 *																	 *
 *	  HARDWARE RESOURCES:									   		 *
 *			 Atmega16m1 Timer 0 is set up as an 8bit PWM	 		 *
 *																	 *
 *	  PIN ASSIGNMENTS:												 *
 *           No external pins 						 				 * 
 *											   						 *
 *    AUTHOR: Stephen Tenniswood, Robotics Engineer 	 			 *
 *		  	  December 2007						 				 	 *
 *		  	  Revised for atmel Sept 2013						 	 *
 *********************************************************************/
//#include "skp_bsp.h"		// include SKP board support package
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "OS_timers.h"
#include "pin_definitions.h"
#include "leds.h"
#include "OS_Dispatch.h"
#include "can_board_msg.h"


// CONSTANTS:


// VARIABLES:
word 	OS_Event_HSTIC_Counter  = 0;		// for high speed sampling mode 0.1ms
int 	OS_Event_TIC_Counter   	= 0;		
short 	OS_Timer_Event_Flag 	= FALSE;
byte 	OS_HighSpeedSampling 	= FALSE;


/*****************************************************************************
Name		: OS_InitTask()   
Parameters	: none
Returns		: nothing      
Description	: Handler for the event tic.
******************************************************************************/
void OS_InitTask()
{
	OS_tmr_init();	
	/******************* PERIPHERAL INIT  ********************/
	/******************* APPLICATION INIT ********************/

	// Look for serial idle:	
	//Enter_HS_Mode();
}

void Enter_HS_Mode()
{
	OS_HighSpeedSampling = TRUE;
}
void Enter_Normal_Mode()
{
	OS_HighSpeedSampling = FALSE;
}

/* Use this dispatch for all core timing functionality */
void System_Dispatch()
{	
	if ((OS_Event_TIC_Counter % 5) == 0) 	// 10ms tasks
	{
		if ((OS_Event_TIC_Counter % 10) == 0) 	// 10ms tasks
		{
			//can_instance_timeslice();
			if ((OS_Event_TIC_Counter % 20) == 0)	// 20ms tasks
			{	
				//can_board_timeslice();
				if ((OS_Event_TIC_Counter % 50) == 0)	// 50ms tasks
				{
					//led_timeslice_100ms();
					if ((OS_Event_TIC_Counter % 100) == 0)	// 100ms tasks
					{						
						if ((OS_Event_TIC_Counter % 1000) == 0)	// 1s tasks
						{
						}
					}
				}
			}
		}
	}
}

/*****************************************************************************
Name		: OS_Timer_isr()
Parameters	: none
Returns		: nothing      
Description	: Handler for the event tic
TIMER0_COMPA_vect
TIMER0_COMPB_vect
TIMER0_OVF_vect
******************************************************************************/
ISR( TIMER0_COMPA_vect )
{
	// the OCF0A flag is automatically cleared by executing this ISR.
	
	// toggle bit used for debug (set an output led to show we're running).
	static byte t =0;
	if (t) t=0; else t=1;		

	//cli():
	// OTHER INTERRUPTS SHOULD BE DISABLED DURING THIS MODE !!
	if (OS_HighSpeedSampling)				// 0.1ms tick
	{
		// call RF check for silence inline routine
		//RF_SilenceCheck();
		if ((OS_Event_HSTIC_Counter++ % 10) == 0)
		{
			OS_Event_HSTIC_Counter = 0;
			OS_Event_TIC_Counter++;
			OS_Dispatch();
		}
	}
	else								// 1ms tick
	{
		OS_Event_TIC_Counter++;
		System_Dispatch();
		OS_Dispatch();
	}
	//if (OS_Event_TIC_Counter == 999)	// after 1 second
    //		OS_Event_TIC_Counter = 0;		// start over
}


/*****************************************************************************
Name:  OS_tmr_init()
Parameters: none
Returns:  nothing      
Description: Setup timer A0 for PWM mode.   
**************************************************************************** */
void OS_tmr_init()
{
	/* FREQ calculation:
	*	 Crystal = 16Mhz
	*		Use f/1 	= 16Mhz
	*		Use f/8 	= 2 Mhz
	*		Use f/64	= 250    khz
	*		Use f/256	= 62.5   khz
	*		Use f/1024	= 15.625 khz
	*
	*  Say we use "f/64" and set OCR0A = 0x250
	*		Gives 1khz signal => 1ms period.
	*/

	/* See Atmel Chapter 16 8 Bit Timer with PWM  (page 126) */	
	
	/* We want to use Clear Timer on Compare (CTC mode)		*/
	/* COM0A0 and COM0A1 are put into "disconnected" mode.	*/
	/* COM0B0 and COM0B1 are put into "disconnected" mode.	*/
	TCCR0A = 0x02;		// CTC;  OCRA will hold TOP;
	TCCR0B = 0x03;		// use f/64
	OCR0A  = 250;		// 1 ms 
	TIMSK0 = 0x02;		// Enable OCR0A
	
	OS_Event_TIC_Counter = 0;	// counts # of ms
}








