#include <stdlib.h>
#include <math.h>
#include "hampel_bandw.h"

static data_t median(data_t *data, int n) {
	// sorting
	int i, j;
	data_t tmp;
	LoopMed1:
	for(i=0; i<n-1; i++){
		LoopMed2:
		for(j=i+1; j<n; j++){
			if(data[i] < data[j]){
			  tmp=data[i];
			  data[i]=data[j];
			  data[j]=tmp;
			}
		}
	}
	return data[n/2];
}

data_t hampel_hw(data_t x, int threshold) {
	int i;
	static data_t buf[WINDOW_SIZE];
	data_t sorted[WINDOW_SIZE];
	data_t med;
	int std;
#pragma HLS ARRAY_PARTITION variable=buf complete
#pragma HLS ARRAY_PARTITION variable=sorted complete

	BufShift:
	for (i = 1; i < WINDOW_SIZE; i++) {
#pragma HLS unroll factor = 7
		buf[i-1] = buf[i];
	}
	buf[WINDOW_SIZE-1] = x;

	copy(buf, sorted, WINDOW_SIZE);
	med = median(sorted, WINDOW_SIZE);
	LoopAbs:
	for (i = 0; i < WINDOW_SIZE; i++) {
#pragma HLS unroll factor = 7
		sorted[i] = abs(sorted[i] - med);
	}
	std = (data_t)148 * median(sorted, WINDOW_SIZE);

	if (abs(buf[WINDOW_SIZE/2]-med) * 100 <= threshold * std) {
		return buf[WINDOW_SIZE/2];
	} else {
		// outlier detected
		//printf("outlier detected!\n");
		return med;
	}
}
