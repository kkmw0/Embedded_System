#include <stdio.h>
#include "Vector_ADT.h"
#include "combine.h"

void combine1(vec_ptr v, long int *dest){
	int i;
	*dest = 0;

	for(i = 0; i < vec_length(v); i++){
		long int val;
		get_vec_element(v, i, &val);
		*dest += val;
	}
}

void combine2(vec_ptr v, long int *dest){
	int i;
	int length = vec_length(v);
	*dest = 0;

	for(i = 0; i < length; i++){
		long int val;
		get_vec_element(v, i, &val);
		*dest += val;
	}
}

void combine3(vec_ptr v, long int *dest){
	int i;
	int length = vec_length(v);
	long int *data = get_vec_start(v);
	*dest = 0;

	for(i = 0; i < length; i++)
		*dest += data[i];
}

void combine4(vec_ptr v, long int *dest){
	int i;
	int length = vec_length(v);
	long int *data = get_vec_start(v);
	long int sum = 0;
	*dest = 0;

	for(i = 0; i < length; i++)
		sum += data[i];
	*dest = sum;
}
