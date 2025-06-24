#ifndef	LIST_H
#define LIST_H

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

typedef struct ListElmt_ ListElmt;

typedef struct List_ List;
 
struct ListElmt_ {
	void * data;
	struct ListElmt_ *next;
	};

struct List_ {
	int size;
	void (*destroy)(void *data);
	ListElmt *head;
	ListElmt *tail;
	};		
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */

#define LIST_SIZE(list)             ((list) != NULL ? (list)->size : 0)

#define LIST_HEAD(list) 			      ((list) != NULL ? (list)->head : NULL)

#define LIST_TAIL(list) 			      ((list) != NULL ? (list)->tail : NULL)

#define LIST_IS_HEAD(list, element) ((element) == (list)->head ? 1 : 0)

#define LIST_IS_TAIL(element)       ((element)->next == NULL ? 1 : 0)

#define LIST_GET_DATA(element)      ((element) != NULL ? (element)->data : NULL)

#define LIST_GET_NEXT_ELMT(element) ((element) != NULL ? (element)->next : NULL)
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

List * InitListEmpty(void (*destroy)(void *data));

void FreeList(void * vptr);

int ListInsertNext(List * list, ListElmt * element, const void * data);

int ListRemoveNext(List * list, ListElmt * element, void ** data);
 
#endif LIST_H		/* end of List.h */
