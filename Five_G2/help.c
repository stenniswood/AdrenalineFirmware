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
#include "bk_system_defs.h"


#define PAUSE_LINE 22

/* Watchdog timer Routines:  */
#define watchdog_reset() __asm__ __volatile__ ("wdr")


const char MSG_1[] PROGMEM = "\r\nKINETIC DRIVE FIVE PROTOCOL : \r\n\r\n";
const char MSG_2[] PROGMEM = "\tuse encoder          - positioning & speed are determined by the encoders\r\n";
const char MSG_3[] PROGMEM = "\tuse potentiometer    - positioning & speed are determined by the pots\r\n";

const char MSG_4[] PROGMEM = "\r\nSETUP COMMANDS: \r\n";
const char MSG_5[] PROGMEM = "\tselect unit (counts|inches|feet|meter) - select active unit\r\n";
const char MSG_6[] PROGMEM = "\tread unit            - display the currently selected unit\r\n";
const char MSG_7[] PROGMEM = "\tset [number] counts_per_unit (v,w,x,y,z) - number of encoder counts \r\n";
const char MSG_8[] PROGMEM = "\t                         that make up the unit (ie 400 counts/inch)\r\n";
const char MSG_9[] PROGMEM = "\tread counts_per_unit - displays counts per unit for all axes;\r\n";
const char MSG_10[] PROGMEM = "\tenable limits (v w x y z all)  - limit switches will stop motor action.\r\n";
const char MSG_11[] PROGMEM = "\tdisable limits (v w x y z all) - limit switches are ignored.\r\n";
const char MSG_11a[] PROGMEM = "\tset Kp v0.035 w0.15 x1.0 y0.5 z20.0 - Proportionality constant for PID control\r\n";
const char MSG_11b[] PROGMEM = "\tset Ki v0.035 w0.15 x1.0 y0.5 z20.0 - Integral constant for PID control\r\n";
const char MSG_11c[] PROGMEM = "\tset Kd v0.035 w0.15 z20.0           - Derivative constant for PID control\r\n";

// 14 lines		don't count extra \r\n the help function sends each line fully.

const char MSG_12[] PROGMEM = "\r\nMOTOR MOVE COMMANDS: \r\n";
const char MSG_13[] PROGMEM = "\tPWM v0.2 w0.5 x0.0 y1.0 z0.5 - set the duty cycle of each axis\r\n";
const char MSG_14[] PROGMEM = "\t                               (values should range from 0.00 to 1.00) \r\n";
const char MSG_15[] PROGMEM = "\tPOS v10500 w9999.5 x500 y450.5 z607 - Move the motors until position\r\n";
const char MSG_15a[] PROGMEM = "\t                                     is reached (PID control).\r\n";
const char MSG_15b[] PROGMEM = "\tset/read destination v[number] w[number] x[number] y[number] z[number]\r\n";
const char MSG_15c[] PROGMEM = "\t           Change/show the motion end point. each axis is optional\r\n";

const char MSG_16[] PROGMEM = "\tspeed v300 w999.5 x200 y450 z607 - Move the motors at specified speed\r\n";
const char MSG_16b[] PROGMEM = "                                    (PID control).\r\n";
const char MSG_17[] PROGMEM = "\thome                 - move all motors until near limit switch is triggered.\r\n";
const char MSG_18[] PROGMEM = "\tmeasure travel (all v w x y z) - measures the counts until far\r\n";
const char MSG_18b[] PROGMEM = "\t					  limit switch is triggered.\r\n";
const char MSG_19[] PROGMEM = "\tstop                 - stop all motors.\r\n";
const char MSG_PAUSE[] PROGMEM = "\r\n--Press Return--\r\n";
// 22 lines

const char MSG_21[] PROGMEM = "\r\nFEEDBACK COMMANDS: \r\n";
const char MSG_22[] PROGMEM = "\tread position        - read the position (based on pots or encoders);\r\n";
const char MSG_23[] PROGMEM = "\tread speed           - speed is deltas of the position;\r\n";
const char MSG_24[] PROGMEM = "\tread current         - the current through each motor;\r\n";
const char MSG_25[] PROGMEM = "\tread status          - okay, over temp, over current, etc;\r\n";
const char MSG_26[] PROGMEM = "\tread base frequency  - the frequency of the pwm drive signals;\r\n";
const char MSG_27[] PROGMEM = "\tset base frequency [number] - the frequency of the pwm drive signals (Hz);\r\n";
const char MSG_28[] PROGMEM = "\tzero encoder (v w x y z all) - mark current positions as 0 counts.\r\n";
const char MSG_28a[] PROGMEM = "\tstream               - Continuously send current positions (toggle).\r\n";

const char MSG_29[] PROGMEM = "\r\n2 WHEELED ROBOT COMMANDS:  v and w axes can be designated for a 2 wheel\r\n";
const char MSG_29b[] PROGMEM = "                              differential robot.\r\n";
const char MSG_30[] PROGMEM = "\tset wheel separation [number] - the distance (in selected unit) between the\r\n";
const char MSG_30b[] PROGMEM = "                                 left & right wheels.\r\n";
const char MSG_31[] PROGMEM = "\tset wheel diameter [number]  - the diameter (in selected unit) of the wheels\r\n";
const char MSG_32[] PROGMEM = "\tset counts_per_rev [number]  - the number of counts per rev for convenience \r\n";
const char MSG_33[] PROGMEM = "\t                      this will update the counts_per_unit configuration value\r\n";

const char MSG_34[] PROGMEM = "\tspin [number] degrees - this instructs it to spin in place ending up facing a\r\n"; 
const char MSG_34b[] PROGMEM = "                         new direction.\r\n";
const char MSG_35[] PROGMEM = "\tforward [number]     - move robot forward __ number of units\r\n"; 
const char MSG_35b[] PROGMEM = "                        (whatever is selected)\r\n";
const char MSG_36[] PROGMEM = "\tbackward [number]    - move robot forward __ number of units\r\n"; 
const char MSG_36b[] PROGMEM = "                        (whatever is selected)\r\n";
const char MSG_37[] PROGMEM = "\tread robot info      - displays the calibrated info about the robot.\r\n";

const char MSG_38[] PROGMEM = "\r\nCONFIGURATION:\r\n";
const char MSG_39[] PROGMEM = "\tsave configuration    - Save all configurtion data to EEPROM.\r\n";
const char MSG_40[] PROGMEM = "\tread configuration    - Read all configurtion data from EEPROM..\r\n";
const char MSG_41[] PROGMEM = "\tdefault configuration - Initialize configurtion data.\r\n";
const char MSG_42[] PROGMEM = "\tread serial number    - for board identification.\r\n";
const char MSG_43[] PROGMEM = "\tread version          - of this firmware.\r\n\r\n";

PGM_P const help_text_table[] PROGMEM = 
{
	MSG_PAUSE, MSG_1, MSG_2, MSG_3, MSG_4, MSG_5, MSG_6, MSG_7, MSG_8, MSG_9, MSG_10,
	MSG_11, MSG_11a, MSG_11b, MSG_11c, 
	MSG_12, MSG_13, MSG_14, MSG_15, MSG_15a, MSG_15b, MSG_15c, MSG_16, MSG_16b, MSG_17, 
	MSG_18, MSG_18b, MSG_19,
	MSG_21, MSG_22, MSG_23, MSG_24, MSG_25, MSG_26, MSG_27, MSG_28, MSG_28a, 
	MSG_29, MSG_29b, MSG_30, MSG_30b,
	MSG_31, MSG_32, MSG_33, MSG_34, MSG_34b, MSG_35, MSG_35b, MSG_36, 
	MSG_36b, MSG_37, MSG_38, MSG_39, MSG_40,
	MSG_41, MSG_42, MSG_43
};

void send_help()
{
	size_t entries = sizeof(help_text_table) / 2;		// 16 bits each pointer!
	RxBuffer.msg_complete = FALSE;		// shouldn't hurt to clear early.	
	for (unsigned char i=1; i<entries; i++)
    {
        strcpy_P(buff, (PGM_P)pgm_read_word(&(help_text_table[i])));
		send_message( buff );     
		if ((i % PAUSE_LINE)==0)  
		{
			// MSG_PAUSE is [0]
	        strcpy_P(buff, (PGM_P)pgm_read_word(&(help_text_table[0])));
			send_message( buff );
			// Pause until '\r' is received.
			while (RxBuffer.msg_complete == FALSE) 
			{
			  			watchdog_reset();
						delay(one_second/10);
						clear_RxBuffer();
			};
		}
    }
	send_prompt();
}

const char LIST_1[] PROGMEM = "\r\nCOMMAND LIST:\r\n";
const char LIST_2[] PROGMEM = "use encoder, use potentiometer, select unit __, read unit, set __ counts_per_unit vwxyz,";
const char LIST_3[] PROGMEM = "read counts_per_unit, enable limits, disable limits, read position, ";
const char LIST_4[] PROGMEM = "read speed, read current, read status, read base frequency, set base frequency, ";
const char LIST_5[] PROGMEM = "zero encoder, pwm, position, speed, home, measure travel, stop, ";
const char LIST_6[] PROGMEM = "set wheel separation, set wheel diameter, set counts_per_rev, set Kp, set Ki,";
const char LIST_7[] PROGMEM = "spin, forward, backward, read robot info, save configuration, read configuration, ";
const char LIST_8[] PROGMEM = "default configuration, read serial number, read version\r\n";

PGM_P const list_text_table[] PROGMEM = 
{
	LIST_1, LIST_2, LIST_3, LIST_4, LIST_5, LIST_6, LIST_7, LIST_8
};

void send_list()
{
	size_t entries = sizeof(list_text_table) / 2;		// 16 bits each pointer!
	RxBuffer.msg_complete = FALSE;		// shouldn't hurt to clear early.	
	for (unsigned char i=0; i<entries; i++)
    {
        strcpy_P(buff, (PGM_P)pgm_read_word(&(list_text_table[i])));
		send_message( buff );       
		if (i==PAUSE_LINE)  {
			// Pause until '\r' is received.
			while (RxBuffer.msg_complete == FALSE) 
			{  			watchdog_reset();
						delay(one_second/10);			};
		}
    }
	send_prompt();
}

