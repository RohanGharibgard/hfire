#ifndef IntTwoDArray_H
#define IntTwoDArray_H
	
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
typedef struct IntTwoDArray_ IntTwoDArray;

struct IntTwoDArray_	{
	int size_rows;
	int size_cols;
	int ** array;
	}; 

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

#define INTTWODARRAY_SIZE_ROW(arr)	 				((arr)->size_rows)

#define INTTWODARRAY_SIZE_COL(arr)	 				((arr)->size_cols)

#define INTTWODARRAY_GET_DATA(arr, row, col)		((arr)->array[(row)][(col)])

#define INTTWODARRAY_SET_DATA(arr, row, col, data)	((arr)->array[(row)][(col)] = (data))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

IntTwoDArray * InitIntTwoDArraySizeEmpty(unsigned int num_rows, unsigned int num_cols);

IntTwoDArray * InitIntTwoDArraySizeIniValue(unsigned int num_rows, unsigned int num_cols, int initial_value);

int ** GetUnderlyingIntTwoDArray(IntTwoDArray * arr);

int GetSizeColIntTwoDArray(IntTwoDArray * arr);

int GetSizeRowIntTwoDArray(IntTwoDArray * arr);
	 
int GetDataIntTwoDArray(IntTwoDArray * arr, unsigned int row, unsigned int col, int * data);

int SetDataIntTwoDArray(IntTwoDArray * arr, unsigned int row, unsigned int col, int data);

void FreeIntTwoDArray(IntTwoDArray * arr);

#endif IntTwoDArray_H		/* end of IntTwoDArray.h */
