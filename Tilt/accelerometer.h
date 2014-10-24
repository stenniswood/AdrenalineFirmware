#ifndef _ACCEL_H_
#define _ACCEL_H_

/***********************************************
* Interface to the LES3DH accelerometer
* Very similar interface to the CMR3000.  
* However, rw & MS bits come first
*
* (msb first - big endian)
* DATE   : 7/27/2013
* AUTHOR : Steve Tenniswood 
***********************************************/
#define STATUS_REG_AUX    0X07
#define OUT_ADC1_L        0X08
#define OUT_ADC1_H        0X09
#define OUT_ADC2_L        0X0A
#define OUT_ADC2_H        0X0B
#define OUT_ADC3_L        0X0C
#define OUT_ADC3_H        0X0D
#define INT_COUNTER_REG   0X0E
#define WHO_AM_I          0X0F
#define TEMP_CFG_REG      0X1F
#define CTRL_REG1         0X20
#define CTRL_REG2         0X21
#define CTRL_REG3         0X22
#define CTRL_REG4         0X23
#define CTRL_REG5         0X24
#define CTRL_REG6         0X25
#define REFERENCE         0X26
#define STATUS_REG2       0X27
#define OUT_X_L           0X28
#define OUT_X_H           0X29
#define OUT_Y_L           0X2A
#define OUT_Y_H           0X2B
#define OUT_Z_L           0X2C
#define OUT_Z_H           0X2D
#define FIFO_CTRL_REG     0X2E
#define FIFO_SRC_REG      0X2F
#define INT1_CFG          0X30
#define INT1_SOURCE       0X31
#define INT1_THIS         0X32
#define INT1_DURATION     0X33
#define CLICK_CFG         0X38
#define CLICK_SRC         0X39
#define CLICK_THS         0X3A
#define TIME_LIMIT        0X3B
#define TIME_LATENCY      0X3C
#define TIME_WINDOW       0X3D
#define DRDY 			  30

#define POWER_DOWN		0x00
#define SAMPLE_1HZ		0x10
#define SAMPLE_10HZ		0x20
#define SAMPLE_25HZ		0x30
#define SAMPLE_50HZ		0x40
#define SAMPLE_100HZ	0x50
#define SAMPLE_200HZ	0x60
#define SAMPLE_400HZ	0x70
#define SAMPLE_1600HZ	0x80
#define SAMPLE_5000HZ	0x90

void accel_data_rate		  ( byte mDataRate );
void accel_temperature_enable ( );
void accel_temperature_disable( );

#define SENSITIVE_2G  0x00
#define SENSITIVE_4G  0x10
#define SENSITIVE_8G  0x20
#define SENSITIVE_16G 0x30
void accel_set_sensitivity( byte Sensitivity );

void swap_bytes		 (  uXYZ* mXYZ  );
void accel_init		 (				);
byte accel_read      ( byte address );
void accel_multi_read( byte address, byte* mData, byte mLength );
void accel_write     ( byte address, byte value );
void accel_bypass_mode	( );
void accel_FIFO_mode	( );
void accel_stream_mode	( );
void accel_stream_to_FIFO_mode( );
void accel_retrieve_mode( );
word accel_read_X		( );
word accel_read_Y		( );
word accel_read_Z		( );
void accel_read_xyz		( uXYZ* mXYZ);
byte accel_new_data_avail( );
void accel_timeslice	( );
byte accel_tests		( );

extern union uXYZ accel;
extern union uXYZ XYZ;


#endif
