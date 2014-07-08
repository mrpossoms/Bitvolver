#ifndef __BITVOLVER
#define __BITVOLVER

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <strings.h>

#define RAND8() ((unsigned char)(rand() % 255))\

/***
 *       _____ _                   _       
 *      / ____| |                 | |      
 *     | (___ | |_ _ __ _   _  ___| |_ ___ 
 *      \___ \| __| '__| | | |/ __| __/ __|
 *      ____) | |_| |  | |_| | (__| |_\__ \
 *     |_____/ \__|_|   \__,_|\___|\__|___/
 *                                         
 *                                         
 */
struct __bitvolver;
typedef struct{
	void*  Generation;
	float* Fitnesses;
	int   MemberCount;
	int   MemberSize;

	float MutationRate;

	void  (*Mutate) (struct __bitvolver*, void* src, void* dst);
	float (*Fitness)(void* member);
} Bitvolver;

/***
 *      ______                _   _                 
 *     |  ____|              | | (_)                
 *     | |__ _   _ _ __   ___| |_ _  ___  _ __  ___ 
 *     |  __| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
 *     | |  | |_| | | | | (__| |_| | (_) | | | \__ \
 *     |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
 *                                                  
 *                                                  
 */
float __randf();
void __binary_mutation(Bitvolver*, void*, void*);
unsigned char* __get_member(void*, int, int);

Bitvolver bitvolver_create(
	int   count,
	int   size,
	float rate,
	void  (*mutate)(Bitvolver*, void*, void*),
	float (*fitness)(void*)
);
/*-----------------------------------------------------------------------*/
void bitvolver_destroy(Bitvolver* bv);
/*-----------------------------------------------------------------------*/
int bitvolver_run(Bitvolver* bv, int threads, int generations);

#endif
