#ifndef NLIBRand_H
	#define NLIBRand_H

	/* ------------------------------------------------------------------- */
	/* Function prototypes: ANSI C library */
	/* ------------------------------------------------------------------- */
	
	#include <math.h>
	#include <stdio.h>		
	#include <stdlib.h>		
	#include <string.h>
	#include <time.h>
	#include <float.h>
	#include <ctype.h>
		
	/* ------------------------------------------------------------------- */
	/* Symbolic constants */
	/* ------------------------------------------------------------------- */
	#define  LRAND_MAX  2147483647L	   /* maximum random integer */
	#define  PI         3.141592654f   /* unit circle area */	
	
	/* Seed the random number generator with the seed s. */
	void randinit (long s);
	
	/* Generate the next random integer in the sequence. */
	long randi (long u);
	
	/* Generate the next random real number uniformly distributed over the interval [a,b]. */
	float randu (float a,float b);
	
	/* Generate the next random real number with a Gaussian distribution with mean m and standard deviation s > 0. */
	float randg (float m,float s);
	
#endif NLIBRand_H	/* end of NLIBRand.h */
