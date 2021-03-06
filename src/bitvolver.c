#include "bitvolver.h"
#include <unistd.h>
#include <stdio.h>

#define RAND8() ((unsigned char)(rand() % 255))\

#define SWAP(arr, t, i, j, size){\
	memcpy(t, &arr[i], size);\
	memcpy(&arr[i], &arr[j], size);\
	memcpy(&arr[j], t, size);\
}\

typedef struct{
	Bitvolver* bv;
	int start;
	int end;
} EvoArgs;

float __randf(){
	unsigned char b = RAND8();
	return b / 255.0f;
}

void __binary_mutation(Bitvolver* bv, void* src, void* dst){
	int i = 0, j = 0;
	int size = bv->MemberSize;
	
	for(i = size; i--;){
		if(__randf() < bv->MutationRate){
			((char*)dst)[i] = ((char*)src)[i] + RAND8();
		}
		else{
			((char*)dst)[i] = ((char*)src)[i];	
		}
	}
	
}

unsigned char* __get_member(void* generation, int i, int size){
	return ((unsigned char*)generation) + i * size;
}

int __partition(Bitvolver* bv, void* tmpMem, int left, int right, int p){
	float pv = bv->Fitnesses[p];
	float t = 0;
	int i, si = left;

	SWAP(bv->Fitnesses, &t, p, right, sizeof(float));
	SWAP(bv->Generation, tmpMem, p, right, bv->MemberSize);

	for(i = left; i < right; i++){
		if(bv->Fitnesses[i] < pv){
				SWAP(bv->Fitnesses, &t, i, si, sizeof(float));
				SWAP(bv->Generation, tmpMem, i, si, bv->MemberSize);
		}
	}
	SWAP(bv->Fitnesses, &t, right, si, sizeof(float));
	SWAP(bv->Generation, tmpMem, right, si, bv->MemberSize);

	return si;
}

int __quickSort(Bitvolver* bv, int left, int right){
	if(left < right){
		int p = (rand() % (right - left - 2)) + left + 1;
		void* tmpMem = malloc(bv->MemberSize); // temp generation member

		p = __partition(bv, tmpMem, left, right, p);

		__quickSort(bv, left, p - 1);
		__quickSort(bv, p + 1, right);

		free(tmpMem); // clean up temp generation member
	}

	return 0;
}

void* __evolve(void* params){
	//printf("evolve entered\n");
	EvoArgs args = *((EvoArgs*)params);
	//printf("cast %d %d\n", args.start, args.end);
	Bitvolver* bv = args.bv;
	int size = bv->MemberSize;
	//printf("reterieved\n");
	int i = 0;

	void* best = NULL;
	int bestIndex = -1;
	float fitness = 0;

	/* find the most fit member */
	for(i = args.start; i <= args.end; i++){
		unsigned char* member = __get_member(bv->Generation, i, size);
		float f = bv->Fitness((void*)member);
		if(f > fitness){
			bestIndex = i;
			fitness = f;
			best = (void*)member;
		}
	}

	write(1, best, size);
	printf("\nMost fit %d @ %0.4f\n\n", bestIndex, fitness);

	/* Copy the most effiecent member over the others */
	for(i = args.start; i <= args.end; i++){
		if(i != bestIndex){
			unsigned char* mem = __get_member(bv->Generation, i, size);
			memcpy((void*)mem, best, size);
		}
	}

	/* Mutate */
	for(i = args.start; i <= args.end; i++){
		if(i != bestIndex){
			unsigned char* member = ((unsigned char*)bv->Generation) + i * size;
			bv->Mutate((struct __bitvolver*)bv, (void*)member, (void*)member);
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
		.Generation   = NULL,
		.Fitnesses    = NULL,
		.MemberCount  = count, /* members */
		.MemberSize   = size,  /* member size in bytes */
		.MutationRate = rate,  /* mutation rate */
		.Mutate  = mutate == NULL ? __binary_mutation : mutate,
		.Fitness = fitness /* fitness calculation function */
	};

	/* seed the rng */
	srand(((unsigned int)time(NULL)) % 1024);

	/* try to allocate memory for the generation's members */
	if(!(out.Generation = malloc(size * count))){
		bzero(&out, sizeof(Bitvolver));
		errno = ENOMEM;
	}

	/* try to allocate memory for the member's fitness values */
	if(!(out.Fitnesses = malloc(sizeof(float) * count))){
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

	//printf("Mem allocated!\n");

	for(j = 0; j < generations; j++){
		int ti = 0;
		int memPerThread = (bv->MemberCount / threads) - 1;
		printf("Generation %d\n", j);
		for(i = threads; i--;){
			args[i].bv = bv;
			args[i].start = ti; ti += memPerThread;
			args[i].end = ti;
			//printf("Thread %d (%d-%d)\n", i, args[i].start, args[i].end);	
			pthread_create(&trds[i], NULL, __evolve, &args[i]);
		}
		//printf("Threads started\n");
		/* Join up on all the threads */
		for(i = threads; i--;){
			pthread_join(trds[i], &returns[i]);
			//printf("Thread %d joined\n", i);
		}
		//printf("Threads finished\n");
	}

	free(trds); /* clean up old threads */
	free(returns);
	free(args);

	return 0;
}

