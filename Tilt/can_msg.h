#ifndef _CANI_MSG_H_
#define _CANI_MSG_H_

struct tXYZ
{
	short x;
	short y;
	short z;
};

union uXYZ 
{
	uint8_t  array[6];	// byte access		
	tXYZ	 coords;	// structure		
	uint16_t value[3];	// xyz as an array	
};

extern struct sCAN tcan;
extern struct sCAN accel_can;
extern struct sCAN gyro_can;
extern struct sCAN magnet_can;

/********************************************************************
 Total Orientation Consists of:
	x,  y, z							Position X, Y, Z 
	dx,dy,dz							Speed in X, Y, Z
	d2x, d2y, d2z						Acceleration in X,Y,Z

	RotateX, 	RotateY,	RotateZ		Angular Position
	dRotateX,	dRotateY,	dRotateZ	Angular Velocity
	d2RotateX,	d2RotateY,	d2RotateZ	Angular Acceleration
	
*********************************************************************/
#define ANALOG_READINGS_BASE_ID 0x0007
#define DEVICE_ID				0x01000000

// FIRMWARE (Major.Minor)  (2.1)
#define MAJOR 			0x01
#define MINOR 			0x00
#define MANUFACTURER 	0x0001				// Beyond Kinetics is Manufacturer #1.

// Each module sends a message to identify itself (type of device, and 
// SerialNumber)
#define AdrenalineEdge_ID		  0x01 
#define AdrenalineTilt_ID  		  0x02 
#define AdrenalineAnalog_ID  	  0x03 
#define AdrenalineButton_ID		  0x04 
#define AdrenalineBlue_ID		  0x05 
#define AdrenalineBigMotor_ID	  0x06 
#define AdrenalineQuadCopter_ID	  0x06 
#define AdrenalinePower_ID		  0x07 
#define SerialNumber	      	  0x56789CDE 

/////////////////// TILT BOARD /////////////////////////
//////////////////// CAN ID'S //////////////////////////
#define   ACCEL_MSG_ID	  			0x0010 
#define   GYRO_MSG_ID	  			0x0011 
#define   MAGNET_MSG_ID	  			0x0012 
#define   TOTAL_ORIENTATION_MSG_ID	0x0013 
////////////////////////////////////////////////////////

void can_prep_generic_XYZ_msg	( sCAN* mMsg, union uXYZ* mAccelData );

// THESE SEND RAW DATA:
void can_prep_accel_msg			( sCAN* mMsg, union uXYZ* mData );
void can_prep_gyro_msg			( sCAN* mMsg, union uXYZ* mData );
void can_prep_magnet_msg		( sCAN* mMsg, union uXYZ* mData );
	
// THESE SEND THE PROCESSED TOTAL ORIENTATION :
void can_prep_total_position_msg	( sCAN* mMsg, union uXYZ* mData );
void can_prep_total_velocity_msg	( sCAN* mMsg, union uXYZ* mData );
void can_prep_total_acceleration_msg( sCAN* mMsg, union uXYZ* mData );

void can_prep_total_angular_position_msg	( sCAN* mMsg, union uXYZ* mData );
void can_prep_total_angular_velocity_msg	( sCAN* mMsg, union uXYZ* mData );
void can_prep_total_angular_acceleration_msg( sCAN* mMsg, union uXYZ* mData );

void can_file_message( sCAN* mMsg );

// SEND THE TOTAL ORIENTATION MATRIX:
//void can_prep_matrix_msg( sCAN* mMsg, union uXYZ mData );
void can_init_test_msg();



#endif
