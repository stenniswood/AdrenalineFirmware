#ifndef _OS_TIMERS_H_
#define _OS_TIMERS_H_

extern int 	OS_Event_TIC_Counter;

void OS_InitTask();
void OS_tmr_init();
void OS_Dispatch();
void OS_timer_isr();
//void OS_Enter_Normal_Mode();
//void OS_Enter_HS_Mode();			// for RF wait for silence sampling.


extern short 	OS_Timer_Event_Flag;
extern int 		Event_TIC_Counter;
extern byte		OS_HighSpeedSampling;
extern void 	Enter_HS_Mode();


#endif
