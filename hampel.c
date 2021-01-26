#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef int data_t;

#define F 5
#define FS 100
#define WINDOW_SIZE 5
#define PREC 65535 // 2**16 sign + 15bit precision


int compare_int(const void *a, const void *b){
    return *(int*)a - *(int*)b;
}

static data_t median(data_t *data, int n) {
	qsort(data, n, sizeof(data_t), compare_int);
	return data[n/2];
}

static void copy(data_t *src, data_t *dest, int n) {
	int i;
	for (i = 0; i < n; i++) dest[i] = src[i];
}


data_t hampel_filter_sw(data_t x, float threshold, int* detected) {
	int i;
	static data_t hampel_buf[WINDOW_SIZE];
	data_t sorted[WINDOW_SIZE];
	data_t med;
	float std;
	//printf("%d\n", hampel_buf[0]);
	for (i = 1; i < WINDOW_SIZE; i++) {
		hampel_buf[i-1] = hampel_buf[i];
	}
	hampel_buf[WINDOW_SIZE-1] = x;

	copy(hampel_buf, sorted, WINDOW_SIZE);
	med = median(sorted, WINDOW_SIZE);
	for (i = 0; i < WINDOW_SIZE; i++) {
		sorted[i] = abs(sorted[i] - med);
	}
	std = 1.4826 * (float)median(sorted, WINDOW_SIZE);
	//printf("std: %f\n", std);
	if ((float)abs(hampel_buf[WINDOW_SIZE/2]-med) <= threshold * std) {
		//return x;
		return hampel_buf[WINDOW_SIZE/2];
	} else {
		// outlier detected
		//printf("outlier detected!\n");
		*detected += 1;
		//hampel_buf[WINDOW_SIZE - 1] = med;
		return med;
	}
}

/*
int main(){
	data_t data[] = {4, 2, 3, 6, 3, 2, 100, 3, 5, 7, 0, 0, 0};
	data_t out;
	for(int i = 0; i<13; i++){
		out = hampel_filter(data[i], 3);
		printf("%d %d \n", data[i], out);
	}
}
*/
int main(){
	int i;
	int error = 0;
	int len = 256;
	int detected = 0;
	float input;
	data_t output;
	
	printf("input output\n");
	for(i = 0; i < len; i++){
		// input data
		input = sin(2 * M_PI * F * i/FS);
		// outliers
		if(i == 10) input = 30;
		if(i == 100) input = -10;
		if(i == 200) input = 5;
		output = hampel_filter_sw((data_t)(input * PREC), 2, &detected);
		printf("%d %d %d\n", i, (data_t)(input * PREC), output);
	}
	printf("%d", detected);
	return 0;
}

