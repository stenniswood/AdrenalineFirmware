#ifndef _CANI_MSG_H_
#define _CANI_MSG_H_


// Each module sends a message to identify itself (type of device, and 
// SerialNumber)
const byte AdrenalineEdge_ID	  = 0x01;
const byte AdrenalineTilt_ID  	  = 0x02;
const byte AdrenalineAnalog_ID    = 0x03;
const byte AdrenalineButton_ID	  = 0x04;
const byte AdrenalineBlue_ID	  = 0x05;
const byte AdrenalineBigMotor_ID  = 0x06;
const byte AdrenalineQuadCopter_ID= 0x06;
const byte AdrenalinePower_ID	  = 0x07;

const long SerialNumber      	  = 0x56789CDE;

void can_file_message			( sCAN* mMsg );
void can_prep_analog_msg		( sCAN* mMsg, uint16_t mReading, byte mMeasurement );
void can_prep_analog_derivative_msg( sCAN* mMsg, uint16_t mReading,    uint16_t mReadingPrev);

void can_send_timeslice();

void can_send_analog_msgs		( );
void can_init_test_msg			( );
// 64 analog readings 16 bits each

#endif
