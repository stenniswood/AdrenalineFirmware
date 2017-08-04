#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include "global.h"
/*#define BOOL unsigned char
#define TRUE 1
#define FALSE 0*/

//#include "differential.h"


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
	BOOL	use_encoder;
	BOOL	use_limits[5];
	float	base_frequency;

	enum eMeasuringUnit  units;
	long int			 counts_per_unit[5];

	float				 wheel_separation;
	float				 wheel_diameter;	
	float				 wheel_circumference;		// calculated quantity
	float				 wheel_counts_per_rev;		// other variables kept in sync with this.

	uint32_t			 encoders_saved;		// count stored when powering down.		
	byte 				 motor_status[5];		// over-current, temp, okay, etc.
	
	unsigned long		 serialNumber;
	int					 FirmwareRevA;
	int					 FirmwareRevB;
	int					 FirmwareRevC;		
};

extern struct stConfigData FiveMotorConfigData;

void init_configuration();

void read_configuration();
void save_configuration();

void  enable_limit( char Letter, BOOL mEnable ) ;

//May0117!
//May117!


#endif
