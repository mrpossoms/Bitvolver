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
		float t = (abs(txt[i] - (int)targ[i]) / 255.0f);
		printf("S: %c - T: %c fit: %f\n", txt[i], targ[i], t);
		fit += t;
	}

	fit /= 12;
	return (1.0f - fit);
}

int main(void){
	
	Bitvolver bv = bitvolver_create(100, 12, 0.5f, NULL, fit);
	//printf("Fitness %f\n", fit(ntar));

	bitvolver_run(&bv, 1, 1);

	return 0;
}
