#ifndef __BITVOLVER
#define __BITVOLVER

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <strings.h>

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
typedef struct __bitvolver Bitvolver;
struct __bitvolver{
	void* Generation;
	int   MemberCount;
	int   MemberSize;

	float MutationRate;

	void  (*Mutate) (Bitvolver*, void* src, void* dst);
	float (*Fitness)(void* member);
}; 
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
