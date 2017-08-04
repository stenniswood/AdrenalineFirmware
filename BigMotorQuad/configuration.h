#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#define BOOL unsigned char
#define TRUE 1
#define FALSE 0


enum eMeasuringUnit
{
	counts,
	millimeters,
	inches
};


/* Global Config Data */
struct stConfigData
{
	BOOL	use_encoder;
	BOOL	use_software_stops;
	enum eMeasuringUnit  units;
	float				 counts_per_inch_M1;
	float				 counts_per_inch_M2;
	float				 counts_per_inch_M3;
	float				 counts_per_inch_M4;
};


extern struct stConfigData QuadMotorConfigData;

void init_configuration();

void read_configuration();
void save_configuration();

#endif
