#ifndef _ANALOG_H_
#define _ANALOG_H_

extern uint8_t 	ActiveSignals[8];		// 2 per analog chip
extern uint16_t	Readings[64];			// Analog Readings (4 chips * 8 = 32)
extern uint16_t	ReadingsPrev[64];		// Analog Readings 

void an_init();				// 
void an_read_actives();		// Read from the Maxim 11624 chips.
void an_apply_filters();	//

void select_chip  ( byte mChip );
void deselect_chip( byte mChip );

void enable_pin_change_ints();
void disable_pin_change_ints();

void an_reset_all		( );
void an_configure_all	( );

#define SCAN_0_to_N 		0x00	//
#define SCAN_N_to_highest 	0x02	// 
#define SCAN_repeat_N 		0x04	// only channel N
#define SCAN_NO_SCAN 		0x06	// ie (1 result from 1 channel)

#define CLK_INT_CNVST	 	0x00	// internal (needs wakeup delay)
#define CLK_INT_CNVST_EXT 	0x10	// externally timed 
#define CLK_INT_AIN 		0x20	// internal
#define CLK_EXTERNAL		0x30	// ie (1 result from 1 channel)

#define REF_OFF_AFTER_READ 	0x00	// internal (needs wakeup delay)
#define REF_EXTERNAL	 	0x04	// external
#define REF_ALWAYS_ON 		0x08	// internal
#define REF_RESERVED 		0x0C	// ie (1 result from 1 channel)

#define AVG_ON		0x10	// 
#define AVG_4 		0x00	// internal (needs wakeup delay)
#define AVG_8	 	0x04	// external
#define AVG_16 		0x05	// internal
#define AVG_32 		0x06	// ie (1 result from 1 channel)

void an_reset	  		( byte mChip );
void an_conversion		( byte mChip, byte mChannel);
void an_setup	  		( byte mChip );
void an_averaging_off	( byte mChip );
void an_averaging_on	( byte mChip, byte mAverageMask );
void an_start_read		( byte mChip, byte mChannels );
void an_start_all		( );
void get_readings		( byte mChip );
byte get_num_channels	( int mChip );


void delay( long int mdelay );
void analog_timeslice	(   );

// See can_msg.h for preping the CAN message buffer with Analog readings

#endif
