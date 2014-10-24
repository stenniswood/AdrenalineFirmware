#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_


void app_default_configuration();

/************* CONFIGURATION *****************/
/**** CONFIG BYTE 1 - BIT DEFS ****/
#define MODE_SEND_COMBINED_ANGLES_VELOCITY   0x01		// 
#define MODE_SEND_COMBINED_LINEAR 			 0x02		// 
#define MODE_SEND_COMBINED_LINEAR_VELOCITY   0x04		// 

/**** CONFIG BYTE 2 - BIT DEFS ****/
/* Upper nibble of config_byte_2 defines the update rate
   Tilt will send accel, gyro and magnet readings
every:
	0	- No report
	1	- Report every  5ms		200Hz	1
	2	- Report every 10ms		100Hz	2
	3	- Report every 20ms		 50Hz	4
	4	- Report every 40ms		 25Hz	8
	F	- Report every 75ms	 	 13Hz  15   */
#define MODE_SEND_UPDATES_NONE		0x00		// 
#define MODE_SEND_UPDATES_10ms		0x10		// 
#define MODE_SEND_UPDATES_20ms		0x20		// 
#define MODE_SEND_UPDATES_30ms		0x40		// 
#define MODE_SEND_UPDATES_50ms		0x80		// 
#define MODE_SEND_ACCEL 			 0x01		// 
#define MODE_SEND_GYRO	 			 0x02		// 
#define MODE_SEND_MAGNET 			 0x04		// 
#define MODE_SEND_COMBINED_ANGLES 	 0x08		// 
byte getReportRate();

/**** CONFIG BYTE 3 - BIT DEFS ****/
// Bits [0,1]
#define MODE_ACCEL_SENSITIVITY_MASK		 0x03		// 
#define MODE_ACCEL_2g	 				 0x00		// 
#define MODE_ACCEL_4g	 				 0x01		// 
#define MODE_ACCEL_8g	 				 0x02		// 
#define MODE_ACCEL_16g	 				 0x03		// 

// Not all of these are implemented.
// Bits [2,3,4]
#define MAGNET_DATA_RATE_MASK	(0x07<<2)
#define MAGNET_DATA_RATE_p75	(0x00<<2)
#define MAGNET_DATA_RATE_1p5	(0x01<<2)
#define MAGNET_DATA_RATE_3		(0x02<<2)
#define MAGNET_DATA_RATE_7p5	(0x03<<2)
#define MAGNET_DATA_RATE_15		(0x04<<2)
#define MAGNET_DATA_RATE_30		(0x05<<2)
#define MAGNET_DATA_RATE_75		(0x06<<2)
#define MAGNET_DATA_RATE_220	(0x07<<2)

// Bits [5,6]
#define MAGNET_AVGS_MASK		0x60
#define MAGNET_AVGS_ONE			0x00
#define MAGNET_AVGS_TWO			0x20
#define MAGNET_AVGS_FOUR		0040
#define MAGNET_AVGS_EIGHT		0060

// Bits [7]
#define MODE_READ_TEMPERATURE	0x80		// uses magnetometer reading

byte getAccelSensitivity() ;
byte getMagnetDataRate()   ;
byte getMagnetAvgs()   	   ;
byte getTemperatureEnable();

void	app_default_configuration( );
byte*	save_cal		();
byte*	read_cal		();
byte	getReportRate	(		   );


#endif
