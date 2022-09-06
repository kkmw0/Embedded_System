#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "Vector_ADT.h"
#include "combine.h"

void get_cpe(vec_ptr v, int len){
	struct timespec tstart, tfinish;
	double CPU_Time;
	double CPU_f = 3593.247 * 1e6;
	double CPE;
	int loops = 1e7;
	int i;

	clock_gettime(CLOCK_MONOTONIC, &tstart);
	for(i = 0; i < loops; i++)
		combine1(v, v->data);
	clock_gettime(CLOCK_MONOTONIC, &tfinish);

	CPU_Time = (tfinish.tv_sec - tstart.tv_sec) + 1e-9 * (tfinish.tv_nsec - tstart.tv_nsec);
	CPE = CPU_Time * CPU_f / loops / len;
	printf("result = %ld\n", v->data[0]);
	printf("CPE = %f\n", CPE);
}

int main(void){
	int len = 100;
	int i;

	vec_ptr result;
	result = new_vec(len);

	for(i = 0; i < len; i++)
		result->data[i] = i;

	get_cpe(result, len);
	return 0;
}
