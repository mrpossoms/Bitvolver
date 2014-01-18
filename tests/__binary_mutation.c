#include <stdio.h>
#include <assert.h>
#include "bitvolver.h"
#define A "ASSERT "
#define I "INSPECT"

int main(void){
	char src[] = "Hello World";
	char dst[] = "Hello World";

	Bitvolver bv = bitvolver_create(
		1,
		12,
		0.1f,
		NULL,
		NULL
	);

	printf("%s\n", I);
	printf("%s\n", src);
	
	__binary_mutation(&bv, src, dst);
	
	printf("%s\n", dst);	

	return 0;
}
