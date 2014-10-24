#ifndef _GYRO_H_
#define _GYRO_H_

/***********************************************
* Interface to the CMR3000 GYRO Registers
* However, rw & MS bits come first
* (msb first - big endian)
***********************************************/
#define GYRO_WHO_AM_I 	0x00
#define REVID     		0x01
#define CTRL      		0x02
#define STATUS    		0x03
#define X_LSB     		0x0C
#define X_MSB     		0x0D
#define Y_LSB     		0x0E
#define Y_MSB     		0x0F
#define Z_LSB     		0x10
#define Z_MSB     		0x11
#define I2C_ADDR  		0x22
#define PDR       		0x26
 
// CTRL REGISTER BIT DEFINITIONS:
#define RESET     		0x80  
#define INT_LEVEL 		0x40  
#define I2C_DIS   		0x10
#define MODE      		0x06
#define INT_DIS   		0x01

// STATUS REGISTER BIT DEFINITIONS:
#define PORST     		0x08
#define PERR      		0x01

extern union uXYZ gyro;

void gyro_isr				(					);
void gyro_init				(					);
byte gyro_read				( byte address		);
void gyro_multi_read		( byte address, byte* mData, byte mLength );
void gyro_write				( byte address, byte value 				  );

void gyro_disable_i2c		(					);
void gyro_reset				(					);
void gyro_set_mode			( byte Mode			);
void gyro_powerdown			(					);
void gyro_standby			(					);
void gyro_measurement_mode	(					);
void gyro_fast_measure_mode	(					);
void gyro_read_xyz			( uXYZ* mData 		);

void gyro_timeslice			(					);
void gyro_tests				(					);


#endif
