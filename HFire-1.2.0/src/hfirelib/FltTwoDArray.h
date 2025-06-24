#ifndef FltTwoDArray_H
#define FltTwoDArray_H
	
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
typedef struct FltTwoDArray_ FltTwoDArray;

struct FltTwoDArray_	{
	int size_rows;
	int size_cols;
	float ** array;
	}; 

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

#define FLTTWODARRAY_SIZE_ROW(arr)	 				((arr)->size_rows)

#define FLTTWODARRAY_SIZE_COL(arr)	 				((arr)->size_cols)

#define FLTTWODARRAY_GET_DATA(arr, row, col)		((arr)->array[(row)][(col)])

#define FLTTWODARRAY_SET_DATA(arr, row, col, data)	((arr)->array[(row)][(col)] = (data))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

FltTwoDArray * InitFltTwoDArraySizeEmpty(unsigned int num_rows, unsigned int num_cols);

FltTwoDArray * InitFltTwoDArraySizeIniValue(unsigned int num_rows, unsigned int num_cols, float initial_value);

float ** GetUnderlyingFltTwoDArray(FltTwoDArray * arr);

int GetSizeColFltTwoDArray(FltTwoDArray * arr);

int GetSizeRowFltTwoDArray(FltTwoDArray * arr);
	 
int GetDataFltTwoDArray(FltTwoDArray * arr, unsigned int row, unsigned int col, float * data);

int SetDataFltTwoDArray(FltTwoDArray * arr, unsigned int row, unsigned int col, float data);

void FreeFltTwoDArray(FltTwoDArray * arr);

#endif FltTwoDArray_H		/* end of FltTwoDArray.h */
