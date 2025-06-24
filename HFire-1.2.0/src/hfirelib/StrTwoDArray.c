#include "StrTwoDArray.h"

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
StrTwoDArray * InitStrTwoDArraySizeEmpty(unsigned int num_rows, unsigned int num_cols)	{
	StrTwoDArray * sa = NULL;
	int i;
	
	if ( num_rows < 1 || num_cols < 1 )	{
		ERR_ERROR_CONTINUE("Unable to initialize array, row and col sizes must be at least 1. \n", ERR_EINVAL);
		return sa;
		}
	if ( (sa = (StrTwoDArray *) malloc(sizeof(StrTwoDArray))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize array, memory allocation failed. \n", ERR_ENOMEM);
		return sa;
		}
	if ( (sa->array = (char ***) malloc(sizeof(char **) * num_rows)) == NULL ) {
		ERR_ERROR_CONTINUE("Unable to initialize array, memory allocation failed. \n", ERR_ENOMEM);
		free(sa);
		sa = NULL;
		return sa;
		}
	for(i = 0; i < num_rows; i++)	{
		if ( (sa->array[i] = (char **) malloc(sizeof(char *) * num_cols)) == NULL )	{
			ERR_ERROR_CONTINUE("Unable to initialize array, memory allocation failed. \n", ERR_ENOMEM);
			/* unwind and deallocate any rows that have been allocated to this point */
			num_rows = i;
			for ( i = 0; i < num_rows; i++)	{
				free(sa->array[i]);
				sa->array[i] = NULL;
				}
			free(sa->array);
			free(sa);
			sa = NULL;
			return sa;			
			}
		}
	sa->size_rows = num_rows;
	sa->size_cols = num_cols;
			
	return sa;
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
StrTwoDArray * InitStrTwoDArraySizeIniValue(unsigned int num_rows, unsigned int num_cols, char * initial_value)	{
	int i, j;
	StrTwoDArray * sa = InitStrTwoDArraySizeEmpty(num_rows, num_cols);
	if ( sa == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize array elements to initial value. \n", ERR_WARNING);	
		return sa;
		}
	for (i = 0; i < num_rows; i++)	{
		for (j = 0; j < num_cols; j++)	{	
			if ( (sa->array[i][j] = (char *) malloc(sizeof(char) * (strlen(initial_value) + 1))) == NULL )	{
				ERR_ERROR_CONTINUE("Unable to allocate memory for string representing initial value. \n", ERR_ENOMEM);
				continue;
				}
			strcpy(sa->array[i][j], initial_value);
			}
		}
	return sa;
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
char *** GetUnderlyingStrTwoDArray(StrTwoDArray * arr)	{
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
int GetSizeColStrTwoDArray(StrTwoDArray * arr)	{
	if ( arr == NULL || arr->array == NULL )	{
		ERR_ERROR_CONTINUE("Unable to retrieve underlying array. \n", ERR_EINVAL);
		return 0;	
		}
	return STRTWODARRAY_SIZE_COL(arr);
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
int GetSizeRowStrTwoDArray(StrTwoDArray * arr)	{
	if ( arr == NULL || arr->array == NULL )	{
		ERR_ERROR_CONTINUE("Unable to retrieve underlying array. \n", ERR_EINVAL);
		return 0;	
		}
	return STRTWODARRAY_SIZE_ROW(arr);
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
int GetDataStrTwoDArray(StrTwoDArray * arr, unsigned int row, unsigned int col, char * data)	{
	if ( arr == NULL || ( row >= arr->size_rows || col >= arr->size_cols ) )	{
		ERR_ERROR("Unable to retrieve data from array, argument invalid.\n", ERR_EINVAL);
		}
	data = STRTWODARRAY_GET_DATA(arr, row, col);
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
 * For the StrTwoDArray, memory allocated for object in row,col will be freed and reallocated
 * for new object at position row,col. Any references to this cell being held prior to this call
 * will become invalid.
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
int SetDataStrTwoDArray(StrTwoDArray * arr, unsigned int row, unsigned int col, char * data)	{
	if ( arr == NULL || ( row >= arr->size_rows || col >= arr->size_cols ) )	{
		ERR_ERROR("Unable to insert data into array, argument invalid.\n", ERR_EINVAL);
		}
	if ( STRTWODARRAY_GET_DATA(arr, row, col) != NULL )	{
		free(STRTWODARRAY_GET_DATA(arr, row, col));
		}
	if ( (STRTWODARRAY_GET_DATA(arr, row, col) = (char *) malloc(sizeof(char) * (strlen(data) + 1))) == NULL )	{
		ERR_ERROR("Unable to allocate memory for string to be inserted at row,col. \n", ERR_ENOMEM);
		}
	strcpy(STRTWODARRAY_GET_DATA(arr, row, col), data);
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
void FreeStrTwoDArray(StrTwoDArray * arr)	{
	int i,j;
	if ( arr != NULL )	{
		if ( arr->array != NULL )	{
			for ( i = 0; i < arr->size_rows; i++)	{
				for( j = 0; j < arr->size_cols; j++)	{
					if ( arr->array[i][j] != NULL )	{
						free(arr->array[i][j]);						/* free a cell in the underlying array */
						}
					}
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
								
/* end of StrTwoDArray.c */
