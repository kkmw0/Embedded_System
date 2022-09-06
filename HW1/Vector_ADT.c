
#include <stdio.h>
#include <stdlib.h>
#include "Vector_ADT.h"

vec_ptr new_vec(int len){
	/* allocate header structure */
	vec_ptr result = (vec_ptr)malloc(sizeof(vec_rec));
	if (!result)
		return NULL; /* Couldn't allocate storage */

	result->len = len;
	/* Allocate array */
	if (len > 0) {
		long int *data = (long int *)calloc(len, sizeof(long int));

		if (!data) {
			free((void *) result);
			return NULL; /* Couldn't allocate storage */
		}
		result->data = data;
	}
	else
		result->data = NULL;

	return result;
}

int get_vec_element(vec_ptr v, int index, long int *dest)
{
	if (index < 0 || index >= v->len)
		return 0;

	*dest = v->data[index];
	return 1;
}

long int *get_vec_start(vec_ptr v){
	return v->data;
}

int vec_length(vec_ptr v){
	return v->len;
}
