#ifndef ByteTwoDArray_H
#define ByteTwoDArray_H
	
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
typedef struct ByteTwoDArray_ ByteTwoDArray;

struct ByteTwoDArray_	{
	int size_rows;
	int size_cols;
	unsigned char ** array;
	}; 

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

#define BYTETWODARRAY_SIZE_ROW(arr)	 				((arr)->size_rows)

#define BYTETWODARRAY_SIZE_COL(arr)	 				((arr)->size_cols)

#define BYTETWODARRAY_GET_DATA(arr, row, col)		((arr)->array[(row)][(col)])

#define BYTETWODARRAY_SET_DATA(arr, row, col, data)	((arr)->array[(row)][(col)] = (data))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

ByteTwoDArray * InitByteTwoDArraySizeEmpty(unsigned int num_rows, unsigned int num_cols);

ByteTwoDArray * InitByteTwoDArraySizeIniValue(unsigned int num_rows, unsigned int num_cols, unsigned char initial_value);

unsigned char ** GetUnderlyingByteTwoDArray(ByteTwoDArray * arr);

int GetSizeColByteTwoDArray(ByteTwoDArray * arr);

int GetSizeRowByteTwoDArray(ByteTwoDArray * arr);
	 
int GetDataByteTwoDArray(ByteTwoDArray * arr, unsigned int row, unsigned int col, unsigned char * data);

int SetDataByteTwoDArray(ByteTwoDArray * arr, unsigned int row, unsigned int col, unsigned char data);

void FreeByteTwoDArray(ByteTwoDArray * arr);

#endif ByteTwoDArray_H		/* end of ByteTwoDArray.h */
