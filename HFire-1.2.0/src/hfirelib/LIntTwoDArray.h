#ifndef LIntTwoDArray_H
#define LIntTwoDArray_H
	
#include <stdlib.h>

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
 
/* structure representing two dimensional array */
typedef struct LIntTwoDArray_ LIntTwoDArray;

struct LIntTwoDArray_	{
	int size_rows;
	int size_cols;
	long int ** array;
	}; 

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

#define LINTTWODARRAY_SIZE_ROW(arr)	 				((arr)->size_rows)

#define LINTTWODARRAY_SIZE_COL(arr)	 				((arr)->size_cols)

#define LINTTWODARRAY_GET_DATA(arr, row, col)		((arr)->array[(row)][(col)])

#define LINTTWODARRAY_SET_DATA(arr, row, col, data)	((arr)->array[(row)][(col)] = (data))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

LIntTwoDArray * InitLIntTwoDArraySizeEmpty(unsigned int num_rows, unsigned int num_cols);

LIntTwoDArray * InitLIntTwoDArraySizeIniValue(unsigned int num_rows, unsigned int num_cols, long int initial_value);

long int ** GetUnderlyingLIntTwoDArray(LIntTwoDArray * arr);

int GetSizeColLIntTwoDArray(LIntTwoDArray * arr);

int GetSizeRowLIntTwoDArray(LIntTwoDArray * arr);
	 
int GetDataLIntTwoDArray(LIntTwoDArray * arr, unsigned int row, unsigned int col, long int * data);

int SetDataLIntTwoDArray(LIntTwoDArray * arr, unsigned int row, unsigned int col, long int data);

void FreeLIntTwoDArray(LIntTwoDArray * arr);

#endif LIntTwoDArray_H		/* end of LIntTwoDArray.h */
