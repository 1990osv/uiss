#ifndef MAIN_ALGORITHM_H
#define	MAIN_ALGORITHM_H

extern volatile unsigned char sod_begin_init;
extern volatile unsigned char sod_first_start;
extern volatile unsigned int time_code;

void loopStart(void);
void loopStop(void);
void query(void);
unsigned char raschet(void);

void sod_init(void);
char sod_raschet(void);

unsigned char current_sod(void);
void update_state_time(void);

#endif
