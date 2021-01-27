#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "hampel_bandw.h"

#define PREC 65535 // 2**16 sign + 15bit precision

int compare_int(const void *a, const void *b){
    return *(int*)a - *(int*)b;
}

static data_t median(data_t *data, int n) {
	qsort(data, n, sizeof(data_t), compare_int);
	return data[n/2];
}

void copy(data_t *src, data_t *dest, int n) {
	int i;
	WindowCopy:
	for (i = 0; i < n; i++) dest[i] = src[i];
}

data_t hampel_filter_sw(data_t x, float threshold, int* detected) {
	int i;
	static data_t buf[WINDOW_SIZE];
	data_t sorted[WINDOW_SIZE];
	data_t med;
	float std;

	for (i = 1; i < WINDOW_SIZE; i++) {
		buf[i-1] = buf[i];
	}
	buf[WINDOW_SIZE-1] = x;

	copy(buf, sorted, WINDOW_SIZE);
	med = median(sorted, WINDOW_SIZE);
	for (i = 0; i < WINDOW_SIZE; i++) {
		sorted[i] = abs(sorted[i] - med);
	}
	std = 1.4826 * (float)median(sorted, WINDOW_SIZE);

	if ((float)abs(buf[WINDOW_SIZE/2]-med) <= threshold * std) {
		return buf[WINDOW_SIZE/2];
	} else {
		// outlier detected
		//printf("outlier detected!\n");
		*detected += 1;
		return med;
	}
}

int main(){
	int i;
	int error = 0;
	int len = 256;
	int detected = 0;
	float input;
	data_t output, out_hw;
	
	printf("input output\n");
	for(i = 0; i < len; i++){
		// input data
		input = sin(2 * M_PI * F * i/FS) / 10;
		// outliers
		if(i == 10) input = 1;
		if(i == 100) input = -1;
		if(i == 200) input = 0.5;

		output = hampel_filter_sw((data_t)(input * PREC), 2, &detected);
		out_hw = hampel_hw((data_t)(input * PREC), 2);
		if(out_hw != output){
			error++;
			printf("%d %d %d\n", i, (int)output, (int)out_hw);
		}
		//printf("%d %d %d\n", i, (data_t)(input * PREC), output);
	}
	printf("detected outliers: %d samples\n", detected);

	if (error)
	    printf("TEST Failed (error: %d)\n", error);
	else
	    printf("TEST Succeeded\n");

	return 0;
}

