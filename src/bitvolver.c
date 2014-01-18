#include "bitvolver.h"

#define RAND8() (random() % 0xFF)\

typedef struct{
	Bitvolver* bv;
	int start;
	int end;
} EvoArgs;

float __randf(){
	return RAND8() / 255.0f;
}

void __binary_mutation(Bitvolver* bv, void* src, void* dst){
	int i = 0, j = 0;
	int size = bv->MemberSize;
	
	for(i = size; i--;){
		if(__randf() < bv->MutationRate){
			((char*)dst)[i] = ((char*)src)[i] + RAND8();
		}
	}
	
}

void* __evolve(void* params){
	EvoArgs args = *((EvoArgs*)params);
	Bitvolver* bv = args.bv;
	int size = bv->MemberSize;
	int i = 0;

	/* Mutate */
	for(i = args.start; i < args.end; i++){
		char* member = ((char*)bv->Generation) + i * size;
		bv->Mutate(bv, (void*)member, (void*)member);
	}
	
	

	return NULL;
}

Bitvolver bitvolver_create(
	int   count,
	int   size,
	float rate,
	void  (*mutate)(Bitvolver*, void*, void*),
	float (*fitness)(void*)){

	Bitvolver out = {
		NULL,
		count, /* members */
		size,  /* member size in bytes */
		rate,  /* mutation rate */
		mutate == NULL ? __binary_mutation : mutate,
		fitness
	};

	/* try to allocate memory for the generation's members */
	if(!(out.Generation = malloc(size * count))){
		bzero(&out, sizeof(Bitvolver));
		errno = ENOMEM;
	}

	return out;
}

void bitvolver_destroy(Bitvolver* bv){
	free(bv->Generation);
}

int bitvolver_run(Bitvolver* bv, int threads, int generations){
	int i = 0, j = 0;
	
	pthread_t* trds = (pthread_t*)malloc(sizeof(pthread_t) * threads);
	void** returns = (void**)malloc(sizeof(void*) * threads);
	EvoArgs* args = (EvoArgs*)malloc(sizeof(EvoArgs) * threads);	

	for(j = 0; j < generations; j++){
		for(i = threads; i--;){
			pthread_create(&trds[i], NULL, __evolve, &args[i]);
		}
		/* Join up on all the threads */
		for(i = threads; i--;){
			pthread_join(trds[i], &returns[i]);
		}
	}

	free(trds); /* clean up old threads */
	free(returns);
	free(args);

	return 0;
}

