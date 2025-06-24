#ifndef	KeyVal_H
#define KeyVal_H

#include <stdlib.h>
#include <string.h>

#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */
 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

typedef struct KeyVal_ KeyVal;

struct KeyVal_	{
	const void * key;
	const void * val;
	};
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

KeyVal * InitKeyVal(const void * key, const void * val);

int MatchStringKeyVal(const void * s1, const void * s2);

void FreeKeyVal(void * vptr);

/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */

int MatchCharBasic(const void * c1, const void * c2);

int MatchIntBasic(const void * i1, const void * i2);
	
int MatchStringBasic(const void * s1, const void * s2);
	 
#endif KeyVal_H		/* end of KeyVal.h */
