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


void send_motor_status()
{
	char* sstr;
	sprintf(buff, "Motor Status: \r\n");
	send_message(buff);

	for (int i=0; i<5; i++) {	
		sstr = get_status_string( FiveMotorConfigData.motor_status[i] );
		sprintf(buff, "%c : %s", motor_letters[i], sstr);
		send_message(buff);
	}

	send_limit_states();
		
	sprintf(buff, "Motor Status: %x %x %x %x %x", 
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
		dtostrf( CurrentSampleAvg[i], 3, 1, temp );
		strcat( buff, temp );
		strcat( buff, " ");
	}	
	form_response( buff );
}


void send_destinations()
{
	for (int i=0; i<5; i++) {
		if (mot_states[i].motor_direction)
			sprintf(buff,"%c  Directoin=+;  Destination=%8ld\r\n", motor_letters[i], mot_states[i].motor_destination );
		else 
			sprintf(buff,"%c  Directoin=-;  Destination=%8ld\r\n", motor_letters[i], mot_states[i].motor_destination );	
		send_message( buff );		
	}
	sprintf(buff,"Motor destinations.");
	form_response( buff );
}

void send_positions()
{
	if (FiveMotorConfigData.use_encoder) {
		sprintf(buff,"Positions: v=%8ld, w=%8ld, x=%8ld, y=%8ld, z=%8ld", 
			Encoders[0].Count, Encoders[1].Count, Encoders[2].Count, Encoders[3].Count, Encoders[4].Count );
	} else {
		sprintf(buff,"Positions: v=%8d, w=%8d, x=%8d, y=%8d, z=%8d", 
			PotSample[0], PotSample[1], PotSample[2], PotSample[3], PotSample[4] );
	}
	form_response( buff );
}

void send_speeds()
{
	if (FiveMotorConfigData.use_encoder) {
		sprintf(buff,"Speeds (cps): v=%8ld, w=%8ld, x=%8ld, y=%8ld, z=%8ld", 
			Encoders[0].Speed, Encoders[1].Speed, Encoders[2].Speed, Encoders[3].Speed, Encoders[4].Speed );
	} else {
		sprintf(buff,"Speeds (cps): v=%8d, w=%8d, x=%8d, y=%8d, z=%8d", 
			PotSample[0], PotSample[1], PotSample[2], PotSample[3], PotSample[4] );	
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

	sprintf(buff, "\r\nuse_encoder = %d\r\n", (FiveMotorConfigData.use_encoder>0) );
	strcat(buff, "unit = " );	
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
/*	sprintf(buff, "Use Limits:\r\n" );
	send_message( buff );	
	for (int m=0; m<5; m++)
	{
		sprintf(buff, "\t%c limit enabled : %d \r\n", motor_letters[m], 
					FiveMotorConfigData.use_limits[m]  );
		send_message( buff );	
	}	*/

	// Uses the 'v' motor for the gearing.
	sprintf( buff, "Counts per rev= " );
	dtostrf( FiveMotorConfigData.wheel_counts_per_rev, 4, 1, temp );
	strcat ( buff,temp );
	strcat ( buff, "\r\n");	
	send_message( buff );
	
	sprintf(buff, " ");		// need a response to give the prompt again.
	form_response(buff);
}


