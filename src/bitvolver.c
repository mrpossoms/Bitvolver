#include "bitvolver.h"

#define RAND8() (random() % 0xFF)\

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

	//printf("Evolve beginning");
	//printf("%d - %d\n", args.start, args.end);

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

	printf("Mem allocated!\n");

	for(j = 0; j < generations; j++){
		int ti = 0;
		int memPerThread = (bv->MemberCount / threads) - 1;
		printf("Generation %d\n", j);
		for(i = threads; i--;){
			args[i].bv = bv;
			args[i].start = ti; ti += memPerThread;
			args[i].end = ti;	
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

