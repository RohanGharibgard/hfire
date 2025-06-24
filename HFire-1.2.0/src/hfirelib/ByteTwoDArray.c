#include "ByteTwoDArray.h"

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
ByteTwoDArray * InitByteTwoDArraySizeEmpty(unsigned int num_rows, unsigned int num_cols)	{
	ByteTwoDArray * ba = NULL;
	int i;
	
	if ( num_rows < 1 || num_cols < 1 )	{
		ERR_ERROR_CONTINUE("Unable to initbalize array, row and col sizes must be at least 1. \n", ERR_EINVAL);
		return ba;
		}
	if ( (ba = (ByteTwoDArray *) malloc(sizeof(ByteTwoDArray))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initbalize array, memory allocation failed. \n", ERR_ENOMEM);
		return ba;
		}
	if ( (ba->array = (unsigned char **) malloc(sizeof(unsigned char *) * num_rows)) == NULL ) {
		ERR_ERROR_CONTINUE("Unable to initbalize array, memory allocation failed. \n", ERR_ENOMEM);
		free(ba);
		ba = NULL;
		return ba;
		}
	for(i = 0; i < num_rows; i++)	{
		if ( (ba->array[i] = (unsigned char *) malloc(sizeof(unsigned char) * num_cols)) == NULL )	{
			ERR_ERROR_CONTINUE("Unable to initbalize array, memory allocation failed. \n", ERR_ENOMEM);
			/* unwind and deallocate any rows that have been allocated to this point */
			num_rows = i;
			for ( i = 0; i < num_rows; i++)	{
				free(ba->array[i]);
				ba->array[i] = NULL;
				}
			free(ba->array);
			free(ba);
			ba = NULL;
			return ba;			
			}
		}
	ba->size_rows = num_rows;
	ba->size_cols = num_cols;
			
	return ba;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Allocate memory for TwoDArray object of user specified dimensions and
 * intbalize all elements in array to user specified value.
 *
 * Arguments:
 * num_rows- number of rows in new TwoDArray
 * num_cols- number of columns in new TwoDArray
 * initbal_value- value assigned to all elements in array
 *
 * Returns:
 * pointer to TwoDArray object with all values set to initbal_value or
 * NULL if memory not able to be allocated
 */
ByteTwoDArray * InitByteTwoDArraySizeIniValue(unsigned int num_rows, unsigned int num_cols, unsigned char initial_value)	{
	int i, j;
	ByteTwoDArray * ba = InitByteTwoDArraySizeEmpty(num_rows, num_cols);
	if ( ba == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initbalize array elements to initbal value. \n", ERR_WARNING);	
		return ba;
		}
	for (i = 0; i < num_rows; i++)	{
		for (j = 0; j < num_cols; j++)	{	
			ba->array[i][j] = initial_value;
			}
		}
	return ba;
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
 * Beware of asynchronous calls to free the TwoDArray structure assocbated with this array, you
 * may suddenly find yourself holding a pointer to memory address 0.
 *
 * Arguments:
 * arr- the TwoDArray object
 *
 * Returns:
 * pointer to underlying two dimensional array
 */
unsigned char ** GetUnderlyingByteTwoDArray(ByteTwoDArray * arr)	{
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
int GetSizeColByteTwoDArray(ByteTwoDArray * arr)	{
	if ( arr == NULL || arr->array == NULL )	{
		ERR_ERROR_CONTINUE("Unable to retrieve underlying array. \n", ERR_EINVAL);
		return 0;	
		}
	return BYTETWODARRAY_SIZE_COL(arr);
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
int GetSizeRowByteTwoDArray(ByteTwoDArray * arr)	{
	if ( arr == NULL || arr->array == NULL )	{
		ERR_ERROR_CONTINUE("Unable to retrieve underlying array. \n", ERR_EINVAL);
		return 0;	
		}
	return BYTETWODARRAY_SIZE_ROW(arr);
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
int GetDataByteTwoDArray(ByteTwoDArray * arr, unsigned int row, unsigned int col, unsigned char * data)	{
	if ( arr == NULL || ( row >= arr->size_rows || col >= arr->size_cols ) )	{
		ERR_ERROR("Unable to retrieve data from array, argument invalid.\n", ERR_EINVAL);
		}
	*data = BYTETWODARRAY_GET_DATA(arr, row, col);
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
int SetDataByteTwoDArray(ByteTwoDArray * arr, unsigned int row, unsigned int col, unsigned char data)	{
	if ( arr == NULL || ( row >= arr->size_rows || col >= arr->size_cols ) )	{
		ERR_ERROR("Unable to insert data into array, argument invalid.\n", ERR_EINVAL);
		}
	BYTETWODARRAY_SET_DATA(arr, row, col, data);
	return ERR_SUCCESS;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Frees memory assocbated with TwoDArray object.
 *
 * Arguments:
 * arr- TwoDArray object to free memory for
 *
 * Returns:
 * None
 */	 	
void FreeByteTwoDArray(ByteTwoDArray * arr)	{
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
								
/* end of ByteTwoDArray.c */
