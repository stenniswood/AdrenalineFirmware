#ifndef _POT_H_
#define _POT_H_

#include "global.h"
//#include "configuration.h"
//#include "calibrations.h"

/*********** TYPE DEFINITIONS *******************************************/
struct sDestination
{
	byte	  continuous;			// 1=> no destination desired - continuous spin.
	uint16_t	  starting_position;	// Used to know which side of the line we started on.  End will be opposite side.  Pot Value	
	uint16_t	  position;			// final position requested
	BOOL	  direction;		// destination compared to starting_position.  true=>Forward
	float 	  speed;
	float 	  speed2;			// for unidirectional motors config.
};

// 10 bit resolution:
#define MAX_POT_VALUE 		   0x03FF
#define MAX_CURRENT_THRESHOLD (0x03FF)

/*********** VARIABLE DEFINITIONS *******************************************/
extern struct sDestination Destination;

extern short PotSpeed [2];
extern uint16_t PotAcceleration[2];

/*********** FUNCTION PROTO-TYPES *******************************************/
void pot_init();	/* This will setup adc_init(), DDR for the Pot & current sense inputs */
void pot_timeslice();

uint16_t 		get_pot();
uint16_t 		get_distance_to_destination();
byte 		get_direction_to_destination();

void 		compute_pot_speed();
void 		compute_pot_acceleration();

float 		convert_to_angle( uint16_t  mPotValue );
uint16_t	 	convert_to_value( long mAngleHundredthDeg );
long int 	convert_to_fixedpoint( float  mRealNumber );

void	  compute_default_stops( );
void 	  compute_range		   ( );

uint16_t	  get_min_stop_value();
long int  get_min_stop_angle();
uint16_t	  get_max_stop_value();
long int  get_max_stop_angle();

long int  get_pot_angle();			// Current Angle
long int  get_current_value();		// Current Pot value

byte	  check_stops( uint16_t  mPotValue );	// Gone past either stop 1 or 2?
byte	  check_stall( uint16_t  mLeftCurrent, uint16_t mRightCurrent );
byte	  destination_reached( );	
void  	  order_stops_by_angle( );
void  	  order_stops_by_value( );

float	  pot_percent();

#endif
