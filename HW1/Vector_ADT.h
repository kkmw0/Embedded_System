#pragma once

typedef struct{
	int len;
	long int *data;
} vec_rec, *vec_ptr;

vec_ptr new_vec(int len);
int get_vec_element(vec_ptr v, int index, long int *dest);
long int *get_vec_start(vec_ptr v);
int vec_length(vec_ptr v);
