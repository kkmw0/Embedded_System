#include <stdio.h>
#include <stdlib.h>
#include "Vector_ADT.h"
#include "combine.h"

int main(void){
	int len = 200;
	int loops = 1e7;
	int i;

	vec_ptr result = new_vec(len);

	for(i = 0; i < loops; i++)
		combine4(result, result->data);

	return 0;
}
