#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#define BOOL unsigned char
#define TRUE 1
#define FALSE 0

#include "differential.h"


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
	BOOL	v_use_limits;
	BOOL	w_use_limits;
	BOOL	x_use_limits;
	BOOL	y_use_limits;
	BOOL	z_use_limits;				
	enum eMeasuringUnit  units;
	long int			 v_counts_per_unit;
	long int			 w_counts_per_unit;
	long int			 x_counts_per_unit;
	long int			 y_counts_per_unit;
	long int			 z_counts_per_unit;
	
	float				 wheel_separation;
	float				 wheel_diameter;	
	float				 wheel_circumference;		// calculated quantity

	uint32_t			 v_encoder_saved;	// count stored when powering down.
	uint32_t			 w_encoder_saved;	// reloaded on reboot.
	uint32_t			 x_encoder_saved;
	uint32_t			 y_encoder_saved;
	uint32_t			 z_encoder_saved;
		
	byte 				 v_status;		// over-current, temp, okay, etc.
	byte 				 w_status;
	byte 				 x_status;
	byte 				 y_status;
	byte 				 z_status;
	
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


#endif
