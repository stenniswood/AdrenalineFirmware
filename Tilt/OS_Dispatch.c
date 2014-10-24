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
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "OS_timers.h"
#include "pin_definitions.h"
#include "leds.h"
#include "can.h"
#include "can_board_msg.h"
#include "magnetometer.h"
#include "gyro.h"
#include "accelerometer.h"
#include "configuration.h"
#include "calibrations.h"
#include "calc_orientation.h"


/*****************************************************************************
Name		:  OS_Dispatch()   
Parameters	:  none
Returns		:  nothing      
Description	:  Dispatches the tasks when their time is due
******************************************************************************/
byte ReportCounter = 1;

void data_gather()
{
	/* Need to know if all three of those can be done in 1 5ms timeslice! 
		We'll estimate it by going by the SPI clock rate & amount of data.
		for accel_timeslice there are ~64 clock periods.	at a rate of 250kbps.  
			= 0.5ms	
			Guestimated to take less than 0.5ms each!
	*/
	accel_timeslice();
	gyro_timeslice ();		// can only go 80Hz or 20Hz!	
	mag_timeslice  ();

/*		if ((isConfigured(MODE_SEND_COMBINED_ANGLES))  ||
			(isConfigured(MODE_SEND_COMBINED_LINEAR)))
			orient_timeslice();			*/ 
}

void OS_Dispatch()
{	
	if ((OS_Event_TIC_Counter % 5) == 0)	// 5ms tasks
	{
		RESET_LED_1();
		RESET_LED_2();
		RESET_LED_3();
		if (getReportRate() != MODE_SEND_UPDATES_NONE)
		{
			if ((--ReportCounter)==0)
			{
				ReportCounter = getReportRate();		// Refill		
				data_gather();
			}
		}
	}
	if ((OS_Event_TIC_Counter % 10) == 0) 	// 10ms tasks
	{	
	}
	if ((OS_Event_TIC_Counter % 33) == 0)	// 33ms tasks
	{	
	
	}
	if ((OS_Event_TIC_Counter % 50) == 0)	// 50ms tasks
	{
	
	}
	if ((OS_Event_TIC_Counter % 500) == 0)	// 100ms tasks
	{
		
	}
	
	/******************************************************************************
	****   PHASE DELAYED OS TASKS:
	******************************************************************************/
//	if (((OS_Event_TIC_Counter+SONAR_PHASE_DELAY) % 50) == 0)	// 50ms tasks
	{
		// This will occur every 25ms behind the normal 50ms task
		// The sonar servo motor must first be moved before we start our ping.
		// This 25ms delay gives the motor sometime to respond.  
		// The motor will move again in the next 25ms.  By then we should have our reading.
		// and any further echo beyond max distance may occur with out disrupting any software
		// If the ping happens to be picked up, so much the better.  If it is not detected,
		// it is beyond the max specified anyway.
	}
}
