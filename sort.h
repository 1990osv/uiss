#ifndef __SORT_H
#define __SORT_H

#define SWAP(A, B) { int t = A; A = B; B = t; } //меняет местами

/*быстрая сортировка
*/
void qs(unsigned int* s_arr, int first, int last) //n - количество элементов
{
    int i = first, j = last, x = s_arr[(first + last) / 2];

    do {
        while (s_arr[i] < x) i++;
        while (s_arr[j] > x) j--;

        if(i <= j) {
            if (s_arr[i] > s_arr[j]) SWAP(s_arr[i], s_arr[j]);
            i++;
            j--;
        }
    } while (i <= j);

    if (i < last)
        qs(s_arr, i, last);
    if (first < j)
        qs(s_arr, first, j);
}

int average(unsigned int* s_arr, unsigned char n) //n - количество элементов
{
	unsigned char i;
	long summ=0;
	
	for (i = 0; i < n; i++){
		summ += s_arr[i];	
	}
	return summ/n;
}
#endif
