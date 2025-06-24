#include "ChHashTable.h"

/*
 * Visibility:
 * global
 *
 * Description:
 * Initializes a Hash Table structure.
 * Provided that Hash Table is initialized with a hash function that uniformly hashes
 * keys supplied to it the performance will be:
 * insertion and deletion from Hash Table in O(1) time
 * random retrieval from Hash Table in O(1) time
 *
 * Arguments:
 * suggest_capacity- suggested capacity of Hash Table, initialization involves calculating
 *						an actual capacity for Hash Table that is the next prime number
 *						larger than suggest_capacity (improves performance of hashing functions)
 * hash_func- ptr to a hashing function that takes a key and capacity of Hash Table as args
 * match- ptr to a function that compares keys and returns 1 if match or 0 otherwise
 * destroy- ptr to function to free memory of keys inserted into Hash Table
 *
 * Returns:
 * NONE
 */		
ChHashTable * InitChHashTable(int suggest_capacity, int (*hash_func)(const void * key, int tbl_sz), 
								int (*match)(const void * key1, const void * key2), 
								void (*destroy)(void *data))	{
	ChHashTable * htable = (ChHashTable *) malloc(sizeof(ChHashTable));
	int i;
	
	/* allocate for hash table */
	if ( htable == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for Hash Table. \n", ERR_ENOMEM);
		return htable;
		}

	/*retrieve next prime number > suggest_capacity */	
	htable->capacity = NextPrimeChHashTable(suggest_capacity);
	
	/* allocate for array of List ptrs */
	if ( (htable->table = (List **) malloc(htable->capacity * sizeof(List *))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for list backing Hash Table. \n", ERR_ENOMEM);
		free(htable);
		return htable;
		}

	/* allocate for individual List elements */
	for(i = 0; i < htable->capacity; i++)	{
		if( (htable->table[i] = InitListEmpty(destroy)) == NULL )	{
			ERR_ERROR_CONTINUE("Unable to allocate memory for list backing Hash Table. \n", ERR_ENOMEM);
			for(i = i - 1; i >= 0; i--)	{
				FreeList(htable->table[i]);
				}
			free(htable);
			return htable;
			}
		}
		
	/* initialize hash members */
	htable->hash_func = hash_func;
	htable->match = match;
	htable->destroy = destroy;
	htable->size = 0;
	
	return htable;		
	}
	
/*
 * Visibility:
 * global
 *
 * Description:
 * Inserts data into Hash Table that is assocated with a user supplied key.
 * Duplicate entries are not allowed, and return an error.
 *
 * Arguments:
 * htable- pointer to a initialized ChHashTable structure
 * key- key used to associate data 
 * data- ptr to data to be inserted
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int ChHashTableInsert(ChHashTable * htable, const void * key, const void * data)	{
	void * insert;
	int hash_val;

	if ( htable == NULL )	{
		ERR_ERROR("Hash Table not initialized, unable to insert data.\n", ERR_EINVAL);	
		}
			 
	/* do nothing if the data is already in the table */
	insert = (void *) data;
	if ( ChHashTableRetrieve(htable, key, &insert) == 0 )	{
		ERR_ERROR("Unable to insert duplicate entries into Hash Table, insert failed.\n", ERR_EINVAL);	
		}

	/* hash the key */
	hash_val = htable->hash_func(key, htable->capacity);

	/* insert the data into the bucket */
	if ( ListInsertNext(htable->table[hash_val], NULL, data) )	{
		ERR_ERROR("Problem inserting element into list backing Hash Table, insert failed.\n", ERR_EINVAL);	
		}	
	htable->size++;

	return ERR_SUCCESS;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Retrieves and removes data from Hash Table using user supplied key.
 *
 * Arguments:
 * htable- pointer to a initialized ChHashTable structure
 * key- key used to associate data  
 * data- if function returns without error, data deleted 
 * 			returned as ptr
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int ChHashTableRemove(ChHashTable * htable, const void * key, void ** data)	{
	ListElmt * element = NULL;
	ListElmt * prev = NULL;
	int hash_val;
	
	if ( htable == NULL )	{
		ERR_ERROR("Hash Table not initialized, unable to remove data.\n", ERR_EINVAL);	
		}

	/* hash the key */
	hash_val = htable->hash_func(key, htable->capacity);
	
	/* search for the data in the table */
	element = LIST_HEAD(htable->table[hash_val]);
	while( element != NULL )	{
		if ( htable->match( key, LIST_GET_DATA(element) ) )	{
			/* remove entry from list */
			if ( ListRemoveNext(htable->table[hash_val], prev, data) )	{
				ERR_ERROR("Unable to remove list element from Hash Table, remove failed. \n", ERR_EBADFUNC);
				}
			htable->size--;
			return ERR_SUCCESS;
			}
		prev = element;
		element = LIST_GET_NEXT_ELMT(element);
		}
	
	ERR_ERROR("Unable find data in Hash Table, remove failed. \n", ERR_EFAILED);
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Retrieves data using user supplied key from Hash Table without deleting it.
 *
 * Arguments:
 * htable- pointer to a initialized ChHashTable structure
 * key- key used to associate data    
 * data- if function returns without error, data retrieved 
 * 			returned as ptr
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int ChHashTableRetrieve(ChHashTable * htable, const void * key, void ** data)	{
	ListElmt * element = NULL;
	int hash_val;
	
	if ( htable == NULL )	{
		ERR_ERROR("Hash Table not initialized, unable to retrieve data.\n", ERR_EINVAL);	
		}

	/* hash the key */
	hash_val = htable->hash_func(key, htable->capacity);
	
	/* search for the data in the table */
	element = LIST_HEAD(htable->table[hash_val]);
	while( element != NULL )	{
		if ( htable->match(key, LIST_GET_DATA(element)) )	{
			/* retrieve data from list */
			*data = LIST_GET_DATA(element);
			return ERR_SUCCESS;
			}
		element = LIST_GET_NEXT_ELMT(element);
		}
	
	return ERR_EFAILED;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Free memory associated with Hash Table.
 *
 * Arguments:
 * htable- pointer to a initialized ChHashTable structure 
 *
 * Returns:
 * NONE
 */			
void FreeChHashTable(ChHashTable * htable)	{
	int i;

	if ( htable != NULL )	{
		if ( htable->table != NULL ) 	{ 
		 	for(i = 0; i < htable->capacity; i++)	{
		 		FreeList(htable->table[i]);									/* free individual Lists */
		 		}		 		
		 	free(htable->table);											/* free array of List ptrs */
			}
		free(htable);
		} 
			
 	htable = NULL;
 	return;
	}

/*
 * Visibility:
 * private
 *
 * Description:
 * Retrieves next prime number > n.
 *
 * Arguments:
 * n- int to retrieve next prime for
 *
 * Returns:
 * Prime number > n
 */	
int NextPrimeChHashTable(int n)	{
	if ( n%2 == 0 )	{
		 n++;
		 }
	for( ; IsPrimeChHashTable(n) ; n+=2)	{
		;	/* null statement */
		}
	return n;
	}

/*
 * Visibility:
 * private
 *
 * Description:
 * Uses trial division technique to test if number is prime.
 *
 * Arguments:
 * n- int to test if prime
 *
 * Returns:
 * Returns 1 if n is prime, 0 otherwise
 */
int IsPrimeChHashTable(int n)		{
	int i;
	for( i = 3; i * i <= n; i += 2)	{
		if ( n%i == 0 )	{
			return 0;
			}
		}
	return 1;
	}
 
/* end of ChHashTable.c */
