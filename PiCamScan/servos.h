#ifndef _SERVOS_H_
#define _SERVOS_H_

#ifdef  __cplusplus
extern "C" {
#endif

/* 
	Typically 20ms period for RC servos.

	Min Pulse Width (PW) ~= 0.5ms
	Max Pulse Width (PW) ~= 2.5ms	
*/
void move_to_max(byte mServoNum);
void move_to_min(byte mServoNum);


#define RC_MIN_ANGLE -60.0
#define RC_MAX_ANGLE +60.0

// These should be modifiable via RPI SPI connection!
#define RC_MIN_PW_TIME_MS 		0.9
#define RC_MAX_PW_TIME_MS		2.1
#define RC_NEUTRAL_PW_TIME_MS  1.5
//#define RC_NEUTRAL_PW_TIME_MS	(((RC_MAX_PW_TIME_MS-RC_MIN_PW_TIME_MS)/2.0)+RC_MIN_PW_TIME_MS)

#define BASE_FREQ				50.0

void servos_init(  );
void set_angle  ( byte mServoNum, float mDegrees);
void neutral    ( byte mServoNum );

#ifdef  __cplusplus
}
#endif

#endif
