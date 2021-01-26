#include <stdlib.h>
#include <math.h>
#include "hampel.h"


void q_sort(int numbers[], int left, int right){
    int pivot, l_hold, r_hold;

    l_hold = left;
    r_hold = right;
    pivot = numbers[left];
    while (left < right){
        while ((numbers[right] >= pivot) && (left < right))
            right--;
        if (left != right){
            numbers[left] = numbers[right];
            left++;
        }
        while ((numbers[left] <= pivot) && (left < right))
            left++;
        if (left != right){
            numbers[right] = numbers[left];
            right--;
        }
    }
    numbers[left] = pivot;
    pivot = left;
    left = l_hold;
    right = r_hold;
    if (left < pivot)
        q_sort(numbers, left, pivot-1);
    if (right > pivot)
        q_sort(numbers, pivot+1, right);
}

static data_t median(data_t *data, int n) {
	// sorting
	int i, j;
	data_t tmp;
	q_sort(data, 0, n);
	return data[n/2];
}

data_t hampel_hw(data_t x, float threshold) {
	int i;
	static data_t buf[WINDOW_SIZE];
	data_t sorted[WINDOW_SIZE];
	data_t med;
	float std;

	BufShift:
	for (i = 1; i < WINDOW_SIZE; i++) {
		buf[i-1] = buf[i];
	}
	buf[WINDOW_SIZE-1] = x;

	copy(buf, sorted, WINDOW_SIZE);
	med = median(sorted, WINDOW_SIZE);
	LoopAbs:
	for (i = 0; i < WINDOW_SIZE; i++) {
		sorted[i] = abs(sorted[i] - med);
	}
	std = 1.4826 * median(sorted, WINDOW_SIZE);

	if ((double)abs(buf[WINDOW_SIZE/2]-med) <= threshold * std) {
		return buf[WINDOW_SIZE/2];
	} else {
		// outlier detected
		//printf("outlier detected!\n");
		return med;
	}
}
