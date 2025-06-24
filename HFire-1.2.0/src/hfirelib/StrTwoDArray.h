#ifndef StrTwoDArray_H
#define StrTwoDArray_H
	
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
 
/* structure representing two dimensional array */
typedef struct StrTwoDArray_ StrTwoDArray;

struct StrTwoDArray_	{
	int size_rows;
	int size_cols;
	char *** array;
	}; 

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

#define STRTWODARRAY_SIZE_ROW(arr)	 				((arr)->size_rows)

#define STRTWODARRAY_SIZE_COL(arr)	 				((arr)->size_cols)

#define STRTWODARRAY_GET_DATA(arr, row, col)		((arr)->array[(row)][(col)])

#define STRTWODARRAY_SET_DATA(arr, row, col, data)	(SetDataStrTwoDArray((arr), (row), (col), (data)))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

StrTwoDArray * InitStrTwoDArraySizeEmpty(unsigned int num_rows, unsigned int num_cols);

StrTwoDArray * InitStrTwoDArraySizeIniValue(unsigned int num_rows, unsigned int num_cols, char * initial_value);

char *** GetUnderlyingStrTwoDArray(StrTwoDArray * arr);

int GetSizeColStrTwoDArray(StrTwoDArray * arr);

int GetSizeRowStrTwoDArray(StrTwoDArray * arr);
 
int GetDataStrTwoDArray(StrTwoDArray * arr, unsigned int row, unsigned int col, char * data);

int SetDataStrTwoDArray(StrTwoDArray * arr, unsigned int row, unsigned int col, char * data);

void FreeStrTwoDArray(StrTwoDArray * arr);

#endif StrTwoDArray_H		/* end of StrTwoDArray.h */
