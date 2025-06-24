#include "List.h"

/*
 * Visibility:
 * global
 *
 * Description:
 * Initializes an empty List.
 * Supports insertion and deletion from list in O(1) time.
 * Searching for a random element in list requires O(N) time.
 *
 * Arguments:
 * Pointer to a function that takes as its argument a data type
 * corresponding to the data that is to be stored in the list.
 * This function allows FreeList to free memory associated with
 * data member of List structure.
 *
 * Returns:
 * Empty List structure.
 */
List * InitListEmpty(void (*destroy)(void *data)) {
	List * list = (List *) malloc(sizeof(List));
	if ( list == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for List. \n", ERR_ENOMEM);
		}
	else	{
		list->size = 0;
		list->destroy = destroy;
		list->head = NULL;
		list->tail = NULL;
		}
		
	return list;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Frees memory associated with all ListElmt comprising List.
 *
 * Arguments:
 * Pointer to List whose ListElemt are to be freed.
 *
 * Returns:
 * NONE.
 */	
void FreeList(void * vptr) {
	List * list = NULL;
	void * data;

	if ( vptr != NULL )	{
 		list = (List *) vptr;	
		while (list->size > 0) {
      if ( ListRemoveNext(list, NULL, (void **) &data) ) {
        ERR_ERROR("Error removing elements from list.\n", ERR_EINVAL);
      }
      if ( list->destroy != NULL && data != NULL )    {
        list->destroy(data);
      }
    }
		free(list);
	}

	list = NULL;
	return;
}
 
/*
 * Visibility:
 * global
 *
 * Description:
 * Inserts data into list in location one past element. If element is NULL then
 * inserts data into head of list.
 *
 * Arguments:
 * list- pointer to a initialized List structure 
 * element- ListElmt after which data will be inserted, if NULL then inserted at head
 * data- data to be inserted
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int ListInsertNext(List * list, ListElmt * element, const void * data) {
	ListElmt * new_element;

	if ( list == NULL )	{
		ERR_ERROR("List not initialized, unable to insert element.\n", ERR_EINVAL);	
		}
		
	if ((new_element = (ListElmt *)malloc(sizeof(ListElmt))) == NULL)	{
		ERR_ERROR("Unable to allocate memory for new element.\n", ERR_ENOMEM);
		}

	new_element->data = (void *)data;

	if (element == NULL) {
		/* Handle insertion at the head of the list. */
		if (list->size == 0)	{
			list->tail = new_element;
      		}
      	new_element->next = list->head;
		list->head = new_element;
		}
	else {
		/* Handle insertion somewhere other than at the head. */
		if (element->next == NULL)	{
			list->tail = new_element;
			}
		new_element->next = element->next;
		element->next = new_element;
		}

	list->size++;

	return ERR_SUCCESS;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Removes data from list in location one past element. If element is NULL then
 * removes data from head of list.
 *
 * Arguments:
 * list- pointer to a initialized List structure 
 * element- ListElmt after which data will be deleted, if NULL then removes data at head
 * data- ptr to data to be deleted is returned in argument
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int ListRemoveNext(List * list, ListElmt * element, void ** data) {
	ListElmt * old_element;

	if ( list == NULL )	{
		ERR_ERROR("List not initialized, unable to remove element.\n", ERR_EINVAL);	
		}
		
	if (list->size == 0)	{
		ERR_ERROR("Cannot remove element from an empty list.\n", ERR_EINVAL);
		}
	
	if (element == NULL) {
	   /* Handle removal from head of list.*/	
		*data = list->head->data;
		old_element = list->head;
		list->head = list->head->next;
		}
	else {
	   /* Handle removal from somewhere other than the head.*/
		if (element->next == NULL)	{
			ERR_ERROR("Cannot remove next element, already at end of list.\n", ERR_EINVAL);
		  	}
		*data = element->next->data;
		old_element = element->next;
		element->next = element->next->next;
		if (element->next == NULL)	{
			list->tail = element;
			}
		}

	free(old_element);

	list->size--;
	if (list->size == 0)	{
    list->head = NULL;
		list->tail = NULL;
		}
	
	return ERR_SUCCESS;
	}

/* end of List.c */
