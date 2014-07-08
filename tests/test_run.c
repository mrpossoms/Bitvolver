#include <stdio.h>
#include <assert.h>
#include "bitvolver.h"
#define A "ASSERT "
#define I "INSPECT"

char targ[] = "Hello World!";
char ntar[] = "Pizza World!";

float fit(void* member){
	int i = 0;
	float fit = 0;
	char* txt = (char*)member;
	for(i = 0; i < 13; i++){
		if(targ[i] == txt[i])
			fit++;
	}
	return fit / 13.0f;
}

int main(void){
	int i = 0;
	char dst[13] = {0};

	Bitvolver bv = bitvolver_create(100, 13, 0.125f, NULL, fit);
	printf("%d Fitness %s %f\n", rand(), ntar, fit(ntar));
	__binary_mutation(&bv, ntar, dst);
	printf("Fitness %s %f\n", dst, fit(dst));

	printf("%f\n", __randf());

	for(i = 0; i < bv.MemberCount * bv.MemberSize; i++){
		((char*)bv.Generation)[i] = 32 + (RAND8() % 122);
	}
	//memcpy(bv.Generation, ntar, 13);
	printf("First member %s", (char*)__get_member(bv.Generation, 0, 13));

	bitvolver_run(&bv, 1, 100);


	return 0;
}
