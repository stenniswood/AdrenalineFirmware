#ifndef _MAGNET_H_
#define _MAGNET_H_

// HMC5983 MAGNETOMETER REGISTERS:
#define CONFIG_REG_A    	 0x00
#define CONFIG_REG_B		 0x01
#define MODE_REGISTER		 0x02
#define DATA_OUTPUT_X_HI     0x03
#define DATA_OUTPUT_X_LO     0x04
#define DATA_OUTPUT_Z_HI     0x05
#define DATA_OUTPUT_Z_LO     0x06
#define DATA_OUTPUT_Y_HI     0x07
#define DATA_OUTPUT_Y_LO     0x08
#define STATUS_REG			 0x09
#define ID_REG_A     		 0x0A
#define ID_REG_B     		 0x0B
#define ID_REG_C			 0x0C
#define TEMPERATURE_OUT_HI     0x49
#define TEMPERATURE_OUT_LO     0x50

#define AVG_1_SAMPLE 0
#define AVG_2_SAMPLE 1
#define AVG_4_SAMPLE 2
#define AVG_8_SAMPLE 3

#define SAMPLE_0_75_HZ	0
#define SAMPLE_1_5_HZ	1
#define SAMPLE_3_HZ		2
#define SAMPLE_7_5_HZ	3
#define SAMPLE_15_HZ	4
#define SAMPLE_30_HZ	5
#define SAMPLE_75_HZ	6
#define SAMPLE_220_HZ	7

#define MIN_GAIN 	0x00
#define MAX_GAIN 	0x07

#define TS 		 	0x80

extern union uXYZ magnet;


/***********************************************
* Interface to the CMR3000 GYRO 
* However, rw & MS bits come first
*
* (msb first - big endian)
***********************************************/
void mag_init 		( 				);
byte mag_read 		( byte address	);
void mag_multi_read	( byte address, byte* mData, byte mLength );
void mag_write		( byte address, byte value );

void mag_enable_temperature ( byte mDataRate );
void mag_disable_temperature( byte mDataRate );

void mag_set_data_rate  ( byte mSampleRate	);
void mag_average_samples( byte mNConst    	);
void mag_set_gain		( byte mGain 		);

void mag_timeslice		( );
void mag_read_xyz		( uXYZ* mXYZ 		);

byte mag_id_tests(	);


#endif
