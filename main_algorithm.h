#ifndef MAIN_ALGORITHM_H
#define	MAIN_ALGORITHM_H

#define SIZE_D  	20			// максимальное количество диапазонов
#define SIZE_TIME  	6			// массив с временами стартовых импульсов
#define SIZE_START_TIME 20			// массив с временами стартовых импульсов



extern unsigned int im[SIZE_D];			// массив с подсчетами
extern unsigned int m[SIZE_D];			// массив с диапазонами (хранит только начало диапазона)

void loop_start(void);
void loop_stop(void);
void query_code(void);
unsigned char raschet(void);

void sod_init(void);
void sod_send_raw_data(void);
char sod_raschet(void);

unsigned char current_sod(void);
void update_state_time(void);

#endif
