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

char* __get_member(void* generation, int i, int size){
	return ((char*)generation) + i * size;
}

void* __evolve(void* params){
	EvoArgs args = *((EvoArgs*)params);
	Bitvolver* bv = args.bv;
	int size = bv->MemberSize;
	int i = 0;

	void* best = NULL;
	int bestIndex = -1;
	float fitness = 0;

	/* find the most fit member */
	for(i = args.start; i < args.end; i++){
		char* member = __get_member(bv->Generation, i, size);
		float f = bv->Fitness((void*)member);

		if(f > fitness){
			bestIndex = i;
			best = (void*)member;
		}
	}

	/* Copy the most effiecent member over the others */
	for(i = args.start; i < args.end; i++){
		if(i != bestIndex){
			char* mem = __get_member(bv->Generation, i, size);
			memcpy((void*)mem, best, size);
		}
	}

	/* Mutate */
	for(i = args.start; i < args.end; i++){
		if(i != best){
			char* member = ((char*)bv->Generation) + i * size;
			bv->Mutate(bv, (void*)member, (void*)member);
		}
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

	printf("Mem allocated!\n");

	for(j = 0; j < generations; j++){
		printf("Generation %d\n", j);
		for(i = threads; i--;){
			pthread_create(&trds[i], NULL, __evolve, &args[i]);
		}
		printf("Threads started\n");
		/* Join up on all the threads */
		for(i = threads; i--;){
			pthread_join(trds[i], &returns[i]);
			printf("Thread %d joined\n", i);
		}
		printf("Threads finished\n");
	}

	free(trds); /* clean up old threads */
	free(returns);
	free(args);

	return 0;
}

