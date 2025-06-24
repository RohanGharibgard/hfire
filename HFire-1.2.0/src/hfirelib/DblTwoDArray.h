#ifndef DblTwoDArray_H
#define DblTwoDArray_H
	
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
typedef struct DblTwoDArray_ DblTwoDArray;

struct DblTwoDArray_	{
	int size_rows;
	int size_cols;
	double ** array;
	}; 

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

#define DBLTWODARRAY_SIZE_ROW(arr)	 				((arr)->size_rows)

#define DBLTWODARRAY_SIZE_COL(arr)	 				((arr)->size_cols)

#define DBLTWODARRAY_GET_DATA(arr, row, col)		((arr)->array[(row)][(col)])

#define DBLTWODARRAY_SET_DATA(arr, row, col, data)	((arr)->array[(row)][(col)] = (data))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

DblTwoDArray * InitDblTwoDArraySizeEmpty(unsigned int num_rows, unsigned int num_cols);

DblTwoDArray * InitDblTwoDArraySizeIniValue(unsigned int num_rows, unsigned int num_cols, double initial_value);

double ** GetUnderlyingDblTwoDArray(DblTwoDArray * arr);

int GetSizeColDblTwoDArray(DblTwoDArray * arr);

int GetSizeRowDblTwoDArray(DblTwoDArray * arr);
	 
int GetDataDblTwoDArray(DblTwoDArray * arr, unsigned int row, unsigned int col, double * data);

int SetDataDblTwoDArray(DblTwoDArray * arr, unsigned int row, unsigned int col, double data);

void FreeDblTwoDArray(DblTwoDArray * arr);

#endif DblTwoDArray_H		/* end of DblTwoDArray.h */
