#ifndef _HAMPEL_H_
#define _HAMPEL_H_
#include "ap_int.h"

#define F 5
#define FS 100
#define WINDOW_SIZE 7

typedef ap_int<17> data_t;

data_t hampel_hw(data_t x, int threshold);

int compare_int(const void *a, const void *b);
void copy(data_t *src, data_t *dest, int n);
#endif

