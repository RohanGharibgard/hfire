#ifndef	ChHashTable_H
#define ChHashTable_H

#include <stdlib.h>

#include "List.h"
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

typedef struct ChHashTable_ ChHashTable;

struct ChHashTable_ {
	int capacity;
	int (*hash_func)(const void * key, int tbl_sz);
	int (*match)(const void * key1, const void * key2);
	void (*destroy)(void *data);
	int size;
	List ** table;
	};
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */

#define CH_HASH_TABLE_SIZE(htable)			((htable)->size)

#define CH_HASH_TABLE_LOAD_FACTOR(htable)	((((htable)->size) / ((htable)->capacity)))
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

ChHashTable * InitChHashTable(int suggest_capacity, int (*hash_func)(const void * key, int tbl_sz), 
								int (*match)(const void * key1, const void * key2), 
								void (*destroy)(void *data));

int ChHashTableInsert(ChHashTable * htable, const void * key, const void * data);

int ChHashTableRemove(ChHashTable * htable, const void * key, void ** data);

int ChHashTableRetrieve(ChHashTable * htable, const void * key, void ** data);
 
void FreeChHashTable(ChHashTable * htable);

/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */
 
int NextPrimeChHashTable(int n);

int IsPrimeChHashTable(int n);
	
#endif ChHashTable_H		/* end of ChHashTable.h */
