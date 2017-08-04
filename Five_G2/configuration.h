#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include "global.h"

#ifndef NUM_MOTORS 
#define NUM_MOTORS 5
#endif


enum eMeasuringUnit
{
	counts,
	inch,
	meter,
	feet
};

/* Global Config Data */
struct stConfigData
{
	BOOL		use_encoder[NUM_MOTORS];
	BOOL		use_limits[NUM_MOTORS];
	float		base_frequency;

	enum eMeasuringUnit  units;
	long int			 counts_per_unit[NUM_MOTORS];
	float				 Kp[NUM_MOTORS];
	float				 Ki[NUM_MOTORS];
	float				 Kd[NUM_MOTORS];
	byte 				 motor_status[NUM_MOTORS];	// over-current, temp, okay, etc.
	
	float				 wheel_separation;
	float				 wheel_diameter;	
	float				 wheel_circumference;		// calculated quantity
	float				 wheel_counts_per_rev;		// other variables kept in sync with this.

	uint32_t			 encoders_saved;			// count stored when powering down.
	
	unsigned long		 serialNumber;
	int					 FirmwareRevA;
	int					 FirmwareRevB;
	int					 FirmwareRevC;		
};

extern struct stConfigData FiveMotorConfigData;

void init_configuration();

void read_configuration();
void save_configuration();

void save_encoders();
void read_encoders();

void read_configuration_and_set();		// sets hardware based on settings (ie. base frequency)

#endif
