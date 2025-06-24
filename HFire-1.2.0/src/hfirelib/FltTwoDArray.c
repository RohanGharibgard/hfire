#include "FltTwoDArray.h"

/*
 * Visibility:
 * global
 *
 * Description:
 * Allocate memory for TwoDArray object of user specified dimensions.
 *
 * Arguments:
 * num_rows- number of rows in new TwoDArray
 * num_cols- number of columns in new TwoDArray
 *
 * Returns:
 * pointer to TwoDArray object or NULL if memory not able to be allocated
 */
FltTwoDArray * InitFltTwoDArraySizeEmpty(unsigned int num_rows, unsigned int num_cols)	{
	FltTwoDArray * da = NULL;
	int i;
	
	if ( num_rows < 1 || num_cols < 1 )	{
		ERR_ERROR_CONTINUE("Unable to initialize array, row and col sizes must be at least 1. \n", ERR_EINVAL);
		return da;
		}
	if ( (da = (FltTwoDArray *) malloc(sizeof(FltTwoDArray))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize array, memory allocation failed. \n", ERR_ENOMEM);
		return da;
		}
	if ( (da->array = (float **) malloc(sizeof(float *) * num_rows)) == NULL ) {
		ERR_ERROR_CONTINUE("Unable to initialize array, memory allocation failed. \n", ERR_ENOMEM);
		free(da);
		da = NULL;
		return da;
		}
	for(i = 0; i < num_rows; i++)	{
		if ( (da->array[i] = (float *) malloc(sizeof(float) * num_cols)) == NULL )	{
			ERR_ERROR_CONTINUE("Unable to initialize array, memory allocation failed. \n", ERR_ENOMEM);
			/* unwind and deallocate any rows that have been allocated to this point */
			num_rows = i;
			for ( i = 0; i < num_rows; i++)	{
				free(da->array[i]);
				da->array[i] = NULL;
				}
			free(da->array);
			free(da);
			da = NULL;
			return da;			
			}
		}
	da->size_rows = num_rows;
	da->size_cols = num_cols;
			
	return da;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Allocate memory for TwoDArray object of user specified dimensions and
 * intialize all elements in array to user specified value.
 *
 * Arguments:
 * num_rows- number of rows in new TwoDArray
 * num_cols- number of columns in new TwoDArray
 * initial_value- value assigned to all elements in array
 *
 * Returns:
 * pointer to TwoDArray object with all values set to initial_value or
 * NULL if memory not able to be allocated
 */
FltTwoDArray * InitFltTwoDArraySizeIniValue(unsigned int num_rows, unsigned int num_cols, float initial_value)	{
	int i, j;
	FltTwoDArray * da = InitFltTwoDArraySizeEmpty(num_rows, num_cols);
	if ( da == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize array elements to initial value. \n", ERR_WARNING);	
		return da;
		}
	for (i = 0; i < num_rows; i++)	{
		for (j = 0; j < num_cols; j++)	{	
			da->array[i][j] = initial_value;
			}
		}
	return da;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Retrieves the underlying array of the TwoDArray object.
 * The primary purpose of this method call is to allow access the array 
 * for getting and setting elements without the penalty of the function calls
 * supplied for doing this in the library.
 * The array pointer returned is the actual underlying array, changes made will be permanent.
 * Beware of asynchronous calls to free the TwoDArray structure associated with this array, you
 * may suddenly find yourself holding a pointer to memory address 0.
 *
 * Arguments:
 * arr- the TwoDArray object
 *
 * Returns:
 * pointer to underlying two dimensional array
 */
float ** GetUnderlyingFltTwoDArray(FltTwoDArray * arr)	{
	if ( arr == NULL || arr->array == NULL )	{
		ERR_ERROR_CONTINUE("Unable to retrieve underlying array. \n", ERR_EINVAL);
		return NULL;
		}
	return arr->array;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Retrieves the size of column dimension of the underlying array.
 *
 * Arguments:
 * arr- the TwoDArray object
 *
 * Returns:
 * size of the column dimension of the underlying array or 0 if array empty
 */
int GetSizeColFltTwoDArray(FltTwoDArray * arr)	{
	if ( arr == NULL || arr->array == NULL )	{
		ERR_ERROR_CONTINUE("Unable to retrieve underlying array. \n", ERR_EINVAL);
		return 0;	
		}
	return FLTTWODARRAY_SIZE_COL(arr);
	}
	
/*
 * Visibility:
 * global
 *
 * Description:
 * Retrieves the size of row dimension of the underlying array.
 *
 * Arguments:
 * arr- the TwoDArray object
 *
 * Returns:
 * size of the row dimension of the underlying array or 0 if array empty
 */
int GetSizeRowFltTwoDArray(FltTwoDArray * arr)	{
	if ( arr == NULL || arr->array == NULL )	{
		ERR_ERROR_CONTINUE("Unable to retrieve underlying array. \n", ERR_EINVAL);
		return 0;	
		}
	return FLTTWODARRAY_SIZE_ROW(arr);
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Retrieves the value at the row and column position in the TwoDArray.  
 * All calls to this method provide for range checking of both row and column indexes.
 * Row and column indexes begin with zero and extend up to but not including size_row, size_col.
 * For example, a TwoDArray object with size_row of 5 has valid row indexes of 0,1,2,3,4.
 *
 * Arguments:
 * arr- the TwoDArray object
 * row- 0-based row index
 * col- 0-based column index
 * data- ptr to var to store retrieved value
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 
int GetDataFltTwoDArray(FltTwoDArray * arr, unsigned int row, unsigned int col, float * data)	{
	if ( arr == NULL || ( row >= arr->size_rows || col >= arr->size_cols ) )	{
		ERR_ERROR("Unable to retrieve data from array, argument invalid.\n", ERR_EINVAL);
		}
	*data = FLTTWODARRAY_GET_DATA(arr, row, col);
	return ERR_SUCCESS;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Inserts the value at the row and column position in the TwoDArray.  
 * All calls to this method provide for range checking of both row and column indexes.
 * Row and column indexes begin with zero and extend up to but not including size_row, size_col.
 * For example, a TwoDArray object with size_row of 5 has valid row indexes of 0,1,2,3,4.
 *
 * Arguments:
 * arr- the TwoDArray object
 * row- 0-based row index
 * col- 0-based column index
 * data- ptr to var to store retrieved value
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 
int SetDataFltTwoDArray(FltTwoDArray * arr, unsigned int row, unsigned int col, float data)	{
	if ( arr == NULL || ( row >= arr->size_rows || col >= arr->size_cols ) )	{
		ERR_ERROR("Unable to insert data into array, argument invalid.\n", ERR_EINVAL);
		}
	FLTTWODARRAY_SET_DATA(arr, row, col, data);
	return ERR_SUCCESS;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Frees memory associated with TwoDArray object.
 *
 * Arguments:
 * arr- TwoDArray object to free memory for
 *
 * Returns:
 * None
 */	 	
void FreeFltTwoDArray(FltTwoDArray * arr)	{
	int i;
	if ( arr != NULL )	{
		if ( arr->array != NULL )	{
			for ( i = 0; i < arr->size_rows; i++)	{
				if ( arr->array[i] != NULL )	{
					free(arr->array[i]);							/* free a row in the underlying array */
					}
				}
			free(arr->array);										/* free the pointers to the rows */
			}
		free(arr);													/* free the TwoDArray structure */
		}
	arr = NULL;
	return;
	}
								
/* end of FltTwoDArray.c */
