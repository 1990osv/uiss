#ifndef SORT_H
#define SORT_H

#define SWAP(A, B) {unsigned int t = A; A = B; B = t; } //меняет местами

#define SIZE_D  20			// максимальное количество диапазонов
extern unsigned int im[SIZE_D];		// массив с подсчетами
extern unsigned int m[SIZE_D];		// массив с диапазонами (хранит только начало диапазона)

void qs(unsigned int* s_arr, int first, int last);
int average(unsigned int* s_arr, unsigned char n);
unsigned int my_filter(unsigned int * arr, unsigned int data_size, unsigned int step);


#endif
