#ifndef TIMERS_H
#define TIMERS_H



#define REG_GTIMER    0
#define DT1_GTIMER    1

#define MAX_GTIMERS  2

void GTimers_Init(void);
void GTimers_Process(void);
void GTimer_Run(unsigned char TIMER_ID);
void GTimer_Pause(unsigned char TIMER_ID);
void GTimer_Release(unsigned char TIMER_ID);
void GTimer_Stop(unsigned char TIMER_ID);
void GTimer_Reset(unsigned char TIMER_ID);
unsigned long GTimer_Get(unsigned char TIMER_ID); //return 0.1 ms


#endif
