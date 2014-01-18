#include <stdio.h>
#include <assert.h>
#include "bitvolver.h"
#define A "ASSERT "
#define I "INSPECT"

int main(void){
	int i = 0;

	for(i = 5; i--;){
		float r = __randf();
		assert(r <= 1.0f && r >= 0.0f);
		printf(". ");
	}
	assert(1 == 1);
	printf(" Passed\n");
	return 0;
}
