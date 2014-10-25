#ifndef _ENCODER_H_
#define _ENCODER_H_

extern word EncoderCount;
extern word EncoderSpeed;
extern word EncoderAcceleration;


void 	  encoder_init();
void	  encoder_disable();


long int  get_encoder_angle();
void 	  encoder_timeslice();




#endif
