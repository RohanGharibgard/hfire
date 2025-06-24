#include "KeyVal.h"

KeyVal * InitKeyVal(const void * key, const void * val)	{
	KeyVal * entry = NULL;
	if ( (entry = (KeyVal *) malloc(sizeof(KeyVal))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for KeyVal. \n", ERR_ENOMEM);
		}
	else	{
		entry->key = key;
		entry->val = val;
		}
	return entry;	
	}

int MatchStringKeyVal(const void * s1, const void * s2)	{
	return MatchStringBasic(s1, ((KeyVal *)s2)->key);
	}
	
void FreeKeyVal(void * vptr)	{
	if ( vptr != NULL )	{
		free(vptr);
		}
	vptr = NULL;
	return;
	}

int MatchCharBasic(const void * c1, const void * c2) {
	return (*(const char *)c1 == *(const char *)c2);
	}

int MatchIntBasic(const void * i1, const void * i2) {
	return (*(const int *)i1 == *(const int *)i2);
	}
	
int MatchStringBasic(const void * s1, const void * s2) {
	return (strcmp((const char *)s1, (const char *)s2) == 0);
	}
	
/* end of KeyVal.c */
