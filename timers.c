#include "global.h"
#include "timers.h"

#define TIMER_STOPPED 0
#define TIMER_RUNNING 1
#define TIMER_PAUSED  2

volatile unsigned long GTimers[MAX_GTIMERS];
volatile unsigned char GTState[MAX_GTIMERS];

void GTimers_Process(void)
{
static unsigned char i=0;
	for(i=0;i<MAX_GTIMERS;i++){
		if(GTState[i]==TIMER_RUNNING){
			GTimers[i]++;
		}
	}
}

void GTimers_Init(void)
{
static unsigned char i=0;
	for(i=0;i<MAX_GTIMERS;i++){
		GTimers[i]=0;
		GTState[i]=TIMER_STOPPED;  
	}                          
}

void GTimer_Run(unsigned char TIMER_ID)
{
	if(GTState[TIMER_ID]==TIMER_STOPPED){
		GTState[TIMER_ID]=TIMER_RUNNING;  
	}
}

void GTimer_Pause(unsigned char TIMER_ID)
{
	if(GTState[TIMER_ID]==TIMER_RUNNING){
		GTState[TIMER_ID]=TIMER_PAUSED;
	}
}

void GTimer_Release(unsigned char TIMER_ID)
{
	if(GTState[TIMER_ID]==TIMER_PAUSED){
		GTState[TIMER_ID]=TIMER_RUNNING;
	}
}

void GTimer_Stop(unsigned char TIMER_ID)
{
	GTState[TIMER_ID]=TIMER_STOPPED;
	GTimers[TIMER_ID]=0;      
}

void GTimer_Reset(unsigned char TIMER_ID)
{
	GTState[TIMER_ID]=TIMER_RUNNING; 
	GTimers[TIMER_ID]=0;      
}

unsigned long GTimer_Get(unsigned char TIMER_ID) // 0.1 ms
{
	return GTimers[TIMER_ID]; 
}



