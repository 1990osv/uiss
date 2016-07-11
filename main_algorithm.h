#ifndef MAIN_ALGORITHM_H
#define	MAIN_ALGORITHM_H

#define SIZE_D  	20			// максимальное количество диапазонов
#define SIZE_TIME  	6			// массив с временами стартовых импульсов
#define SIZE_START_TIME 20			// массив с временами стартовых импульсов

#define STORE_SIZE	100			//величина буфера для обработки

extern unsigned int rawStoreTimeCode[STORE_SIZE]; 	//массив не отсортированных кодов 

extern unsigned int im[SIZE_D];			// массив с подсчетами
extern unsigned int m[SIZE_D];			// массив с диапазонами (хранит только начало диапазона)

void startGeneralLoop(void);
void stopGeneralLoop(void);
void generalLoop(void);
unsigned char raschet(void);

void contentInitialisation(void);
void sod_refreshRawData(void);
void computeContent(void);
void start_digitize(void);


unsigned char current_sod(void);
void update_state_time(void);

#endif
