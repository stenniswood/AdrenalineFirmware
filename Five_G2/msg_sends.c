#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "protocol.h"
#include "encoder.h"
#include "configuration.h"
#include "adc.h"
#include "serial.h"
#include "pwm.h"
#include "motor.h"
#include "differential.h"

#include "limit_switches.h"
#include "leds2.h"
#include "bk_system_defs.h"
#include "msg_sends.h"
#include "pid_controller.h"

void send_motor_status()
{
	char* sstr;
	sprintf(buff, "Motor Status: \r\n");
	send_message(buff);

	for (int i=0; i<5; i++) {	
		sstr = get_status_string( FiveMotorConfigData.motor_status[i] );
		sprintf(buff, "%c:%s  ", motor_letters[i], sstr);
		send_message(buff);
	}
	strcpy(buff, "\r\n");	
	send_message(buff);

	send_limit_states();
		
	sprintf(buff, "Raw Status: %x %x %x %x %x", 
						FiveMotorConfigData.motor_status[0], 
						FiveMotorConfigData.motor_status[1],
						FiveMotorConfigData.motor_status[2],
						FiveMotorConfigData.motor_status[3],
						FiveMotorConfigData.motor_status[4]  );
	strcat(buff, "\r\n");	
	form_response(buff);
}


void send_current_readings()
{
	char temp[10];
		
	sprintf(buff, "Current: ");
	for (int i=0; i<5; i++)
	{
		//dtostrf( CurrentSample[i], 3, 1, temp );
		sprintf(temp, " %d ", CurrentSample[i] );
		strcat( buff, temp );
		//strcat( buff, " ");
	}	
	form_response( buff );
}


void send_destinations()
{
	for (int i=0; i<5; i++) {
		if (mot_states[i].direction)
			sprintf(buff,"%c  Directoin=+;  Destination=%8ld\r\n", motor_letters[i], mot_states[i].destination );
		else 
			sprintf(buff,"%c  Directoin=-;  Destination=%8ld\r\n", motor_letters[i], mot_states[i].destination );	
		send_message( buff );		
	}
	sprintf(buff,"Motor destinations.");
	form_response( buff );
}

void send_positions()
{
	char temp[20];
	sprintf(buff,"Positions: ");
	for (int m=0; m<NUM_MOTORS; m++)
	{
		if (FiveMotorConfigData.use_encoder[m]) {
			sprintf ( temp, "%c=%8ld, ", motor_letters[m], Encoders[m].Count );
			strcat  ( buff, temp);
		} else {
			sprintf ( temp,"%c=%8d, ", motor_letters[m], PotSample[m] );
			strcat(buff, temp);
		}
	}
	form_response( buff );
}

void send_speeds()
{
	char temp[20];
	sprintf(buff,"Speeds (cps): ");
	for (int m=0; m<NUM_MOTORS; m++)
	{
		if (FiveMotorConfigData.use_encoder[m]) {			
			sprintf(temp,"%c=%8ld, ", motor_letters[m], Encoders[m].Speed );
			strcat(buff, temp);
		} else {
			sprintf(temp,"%c=%8d ", motor_letters[m], PotSample[m] );
			strcat(buff, temp);
		}
	}
	form_response( buff );	
}

void send_base_frequency()
{
	float num = get_base_frequency_herz();
	char temp[10];
	dtostrf( num, 4, 1, temp );
	sprintf(buff,"Base Frequency (hz): %s", temp );
	form_response(buff);
}

void send_robot_info()
{
	char temp[40];
	
	print_encoder_selections();
		
	
	strcpy(buff, "\r\nunit = " );	
	strcat(buff, unit_to_string() );
	strcat(buff, "\r\n");
	send_message( buff );

	sprintf(buff, "Base Frequency = " );	
	dtostrf( FiveMotorConfigData.base_frequency, 3, 1, temp );
	strcat(buff, temp );	
	strcat(buff, "\r\n");
	send_message( buff );

	sprintf(buff, "Wheel diameter= " );	
	dtostrf( FiveMotorConfigData.wheel_diameter, 3, 1, temp );
	strcat(buff, temp );	
	strcat(buff, unit_to_string() );
	strcat(buff, "\r\n");
	send_message( buff );
	
	sprintf(buff, "Wheel separation= " );	
	dtostrf( FiveMotorConfigData.wheel_separation, 3, 1, temp );
	strcat(buff, temp );	
	strcat(buff, unit_to_string() );
	strcat(buff, "\r\n");
	send_message( buff );
	
	sprintf(buff, "Wheel circumference= " );	
	// calc now b/c may have been stored under different unit.
	float circumf = FiveMotorConfigData.wheel_diameter * M_PI;
	dtostrf( circumf, 3, 1, temp );
	strcat(buff, temp );	
	strcat(buff, unit_to_string() );
	strcat(buff, "\r\n");
	send_message( buff );

	// List all unit to counts ratios:
	for (int m=0; m<5; m++)
	{
		sprintf(buff, "\t%c counts_per_unit=%ld %s\r\n", motor_letters[m], 
					FiveMotorConfigData.counts_per_unit[m], 
					unit_to_string() );
		send_message( buff );		
	}
	
	// List all motor status':
/*	sprintf(buff, "Motor Status':\r\n" );
	send_message( buff );	
	for (int m=0; m<5; m++)
	{
		sprintf(buff, "\t%c motor_status=%2x \r\n", motor_letters[m], 
					FiveMotorConfigData.motor_status[m]  );
		send_message( buff );		
	}*/

	// List Limit enable status:
	send_limit_states();

	// Uses the 'v' motor for the gearing.
	sprintf( buff, "Counts per rev= " );
	dtostrf( FiveMotorConfigData.wheel_counts_per_rev, 4, 1, temp );
	strcat ( buff,temp );
	strcat ( buff, "\r\n");	
	send_message( buff );

	print_pid_parameters();
	
	
	sprintf(buff, "\r\n");		// need a response to give the prompt again.
	form_response(buff);
}


