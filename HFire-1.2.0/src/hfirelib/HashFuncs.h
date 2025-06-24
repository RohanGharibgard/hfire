#ifndef	HashFuncs_H
#define HashFuncs_H

#include <stdlib.h>
#include <math.h>

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */
 
#define HASH_FUNCS_OLD_FASHIONED_POW_VALUE				(127)

#define HASH_FUNCS_STR_TO_NUM_POW_VALUE					(8)
 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */
 	
#define HASH_FUNCS_GET_NUM_BITS(x)						((log((x))/log(2))+1)
	
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

int SimpleDivisionIntHashFunc(const void * key, int tbl_sz);

int OldFashionedStringHashFunc(const void * key, int tbl_sz);

int UniversalStringHashFunc(const void * key, int tbl_sz);

int WeinbergerStringHashFunc(const void * key, int tbl_sz);

int ShiftFoldingIntHashFunc(const void * key, int tbl_sz);

int ShiftFoldingStringHashFunc(const void * key, int tbl_sz);

int MiddleBitsIntHashFunc(const void * key, int tbl_sz);

int MiddleBitsStringHashFunc(const void * key, int tbl_sz);

/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */
 
int StringToNumberForHashFunc(const char * c);
 
#endif HashFuncs_H		/* end of HashFuncs.h */
