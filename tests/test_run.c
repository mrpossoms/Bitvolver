#include <stdio.h>
#include <assert.h>
#include "bitvolver.h"
#define A "ASSERT "
#define I "INSPECT"

char targ[] = "Hello World!";
char ntar[] = "Bello Borld!";

float fit(void* member){
	int i = 0;
	float fit = 0;
	char* txt = (char*)member;
	for(i = 0; i < 12; i++){
		if(targ[i] == txt[i])
			fit++;
	}

	return fit / 12.0f;
}

int main(void){
	int i = 0;
	Bitvolver bv = bitvolver_create(100, 12, 0.25f, NULL, fit);
	//printf("Fitness %f\n", fit(ntar));

	for(i = 0; i < bv.MemberCount * bv.MemberSize; i++){
		((char*)bv.Generation)[i] = (char)(random() % 255);
	}

	bitvolver_run(&bv, 1, 1000);
	printf("%s", (char*)bv.Generation);

	return 0;
}
