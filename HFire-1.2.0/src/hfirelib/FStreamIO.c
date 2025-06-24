#include "FStreamIO.h"

/*
 * Visibility:
 * global
 *
 * Description:
 * Gets the variably sized current line pointed to by fstream, char-by-char
 * This method is rather versatile in that it can read from any type of stream such as files or stdin.
 * The user is responsible for freeing memory associated with the returned char array.
 * ALWAYS call free on the returned character array to prevent memory leaks.
 * Upon return from this function, the fstream will point to one line below the location in the file
 * where the fstream pointed to when the function was first called. 
 *
 * Arguments:
 * fstream- FILE pointer to an open and readable stream
 *
 * Returns:
 * C-style string ('\0' terminated array of characters) corresponding to data
 * at current line in stream.
 * User is responsible for freeing memory associated with returned string. 
 */
char * GetLineFStreamIO(FILE * fstream)	{
	char * line = NULL;
	int c, i = 0, ini_size_line = FSTREAMIO_INITIAL_SCAN_LINE_SIZE;
	
	/* check args */
	if ( fstream == NULL )	{
		ERR_ERROR_CONTINUE("Problems reading from stream passed to GetLineFStreamIO. \n", ERR_EIOFAIL);
		return line;
		}	
	if ( (line = (char *) malloc( sizeof(char) * ini_size_line )) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for scan line GetLineFStreamIO. \n", ERR_ENOMEM);
		return line;
		}
		
	/* start ingesting line */
	while(1)	{
		if ( i == ini_size_line )	{
			/* dynamically expand size of line we can read */
			ini_size_line *= 2;
			if ( (line = (char *) realloc(line, sizeof(char) * ini_size_line)) == NULL )	{
				ERR_ERROR_CONTINUE("Memory not allocated for scan line GetLineFStreamIO. \n", ERR_ENOMEM);
				break;
				}
			}		
		c = fgetc(fstream);
		if ( c == '\n' || c == '\r' || c == EOF )	{
      /* read and discard newline following carriage return */
      if ( c == '\r' )    {
          fgetc(fstream);
      }
			/* nothing else to read */
			line[i] = '\0';
			break;
			}
		line[i] = c;
		i++;
		}
	
	/* editted MM 07032001 */
	/* realloc(line, sizeof(char) * i); */
	return line;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Takes an open and readable file stream and parses lines from file not preceded 
 * by comments into a List.  The List is returned to the user.  
 * Each line in the stream is retrieved by successively calling GetLineFStreamIO.
 * Lines beginning with any of the characters in the array of comment characters
 * are not added to the List.
 * Upon return from this function, the fstream will point to the end of file. 
 *
 * Arguments:
 * fstream- FILE pointer to open and readable stream
 * cmtchr- array fo characters representing valid comment characters
 *			if NULL then all lines are added to List
 *
 * Returns:
 * List of values parsed from stream or NULL if not initialized.
 * User is responsible for freeing memory associated with returned List.
 */  
List * GetListNonCommentLinesFStreamIO(FILE * fstream, const char * cmtchr)	{
	List * line_list		= NULL;						/* List of lines in stream */
	ListElmt * line_elmt 	= NULL;						/* single element in line List */
	char * line				= NULL;						/* line parsed from stream */
	const char * cmt 		= NULL;						/* ptr used to iterate over comment char array */
	int has_comment = 0;
	
	/* check args */
	if ( fstream == NULL )	{
		ERR_ERROR_CONTINUE("Unable to read stream passed to GetListNonCommentLinesFStreamIO. \n", ERR_EIOFAIL);
		return line_list;
		}

	/* initialize List to store lines from stream */
	if ( (line_list = InitListEmpty(free)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory for List of lines not allocated in GetListNonCommentLinesFStreamIO. \n", ERR_ENOMEM);
		return line_list;
		}
		
	/* place lines from stream in List until end of file reached */
	while ( (line = GetLineFStreamIO(fstream)) != NULL )		{
		has_comment = 0;
		/* test if line begins with comment or non-printing character */
		for(cmt = cmtchr; *cmt != '\0'; cmt++)	{
			if ( line[0] == *cmt || isgraph(line[0]) == 0 )	{
				has_comment = 1;
				}
			}
		/* add line to list */
		if ( has_comment == 0 )	{
			line_elmt = LIST_TAIL(line_list);
			if ( ListInsertNext(line_list, line_elmt, line) )	{
				ERR_ERROR_CONTINUE("Problem inserting line into List. \n", ERR_EBADFUNC);
				}
			}
		/* free memory and discard line */
		else	{
			free(line);
			}
		/* test if more data in stream */
		if ( feof(fstream) )	{
			break;
			}
		}
	
	return line_list;	
	}
	
/*
 * Visibility:
 * global
 *
 * Description:
 * Takes an open and readable file stream and searches for first non-comment line
 * with string matching the supplied key.  If key string is not found then function
 * returns NULL. Otherwise, the line containing key string is parsed into tokens
 * delimited by sepchr. The first token following the matching key is copied into heap
 * allocated memory and returned as a C-style string ('\0' terminated array of characters).
 * If no token associated with key is found, then function returns NULL ptr.
 * Upon return from this function, the fstream will point to one line below the location in
 * the file where the key searched for was found, or the EOF if key not found.    
 *
 * Arguments:
 * fstream- FILE pointer to open and readable stream
 * key- null terminated string corresponding to key to search stream for
 * sepchr- array of characters delimiting key from value
 * cmtchr- array fo characters representing valid comment characters
 *
 * Returns:
 * C-style string ('\0' terminated array of characters) corresponding to value
 * associated with key searched for or NULL if key not found or no value associated with key.
 * User is responsible for freeing memory associated with returned string.
 */
char * GetValFromKeyStringFStreamIO(FILE * fstream, const char * keystr, const char * sepchr, const char * cmtchr)	{
	char * value 		= NULL;						/* returned value as string */
	char * line 		= NULL;						/* line read from stream */
	char * token 		= NULL;						/* tokenizer */
	const char * cmt 	= NULL;						/* ptr used to iterate over comment chars */
	int has_comment = 0;
		
	/* check args */
	if ( fstream == NULL )	{
		ERR_ERROR_CONTINUE("Problems reading from stream passed to GetValFromKeyStringFStreamIO. \n", ERR_EIOFAIL);
		return value;
		}		

	/* retrieve lines until end of file reached */
	while ( (line = GetLineFStreamIO(fstream)) != NULL )		{
		has_comment = 0;		
		/* test if line begins with comment or non-printing character */
		for(cmt = cmtchr; *cmt != '\0'; cmt++)	{
			if ( line[0] == *cmt || isgraph(line[0]) == 0 )	{
				has_comment = 1;
				}
			}		
		/* non-comment line, test if key matches searched for key */
		if ( has_comment == 0 )	{
			if ( (token  = strtok(line, sepchr)) != NULL  && (strcmp(token, keystr)) == 0 )	{
				/* parse value associated with key, if no value function returns NULL */
				if ( (token = strtok(NULL, sepchr)) != NULL )	{					
					if ( (value = (char *) malloc(sizeof(char) * (strlen(token) + 1))) != NULL )	{
						strcpy(value, token);
						}
					else	{
						ERR_ERROR_CONTINUE("Memory not allocated in GetValFromKeyStringFStreamIO. \n", ERR_ENOMEM);
						}					
					}
				free(line);
				break;
				}
			}
		free(line);
		/* test if more data in stream */
		if ( feof(fstream) )	{
			break;
			}		
		}
				
	return value;
	}	

/*
 * Visibility:
 * global
 *
 * Description:
 * Takes an open and readable file stream and searches for first non-comment line
 * with string matching the supplied key.  If key string is not found then function
 * returns 0 (false). Upon return from this function, the fstream will point to one
 * line below the location in the file where the key searched for was found, or the 
 * EOF if key not found.    
 *
 * Arguments:
 * fstream- FILE pointer to open and readable stream
 * key- null terminated string corresponding to key to search stream for
 * cmtchr- array fo characters representing valid comment characters
 *
 * Returns:
 * 1 (true) if the key was found.
 * 0 (false) if the key was not found.
 */
int IsKeyStringPresentFStreamIO(FILE * fstream, const char * keystr, const char * cmtchr)	{
	char * line 		= NULL;						/* line read from stream */
	const char * cmt 	= NULL;						/* ptr used to iterate over comment chars */
	int has_comment = 0;
		
	/* check args */
	if ( fstream == NULL )	{
		ERR_ERROR_CONTINUE("Problems reading from stream passed to GetValFromKeyStringFStreamIO. \n", ERR_EIOFAIL);
		return 0;
		}		

	/* retrieve lines until end of file reached */
	while ( (line = GetLineFStreamIO(fstream)) != NULL )		{
		has_comment = 0;		
		/* test if line begins with comment or non-printing character */
		for(cmt = cmtchr; *cmt != '\0'; cmt++)	{
			if ( line[0] == *cmt || isgraph(line[0]) == 0 )	{
				has_comment = 1;
				}
			}		
		/* non-comment line, test if key matches searched for key */
		if ( (has_comment == 0) && (strcmp(line, keystr) == 0) )	{
			free(line);
			return 1;
			}
		free(line);
		/* test if more data in stream */
		if ( feof(fstream) )	{
			break;
			}		
		}
				
	return 0;
	}
	 
/*
 * Visibility:
 * global
 *
 * Description:
 * Takes a C-style string ('\0' terminated array of characters) and tokenizes the
 * string using any of the characters in sepchr as delimeters.
 * Each of the tokenized substrings are converted to type double and inserted
 * inside of a dynamically sized list.
 * The function terminates when the end of the string has been reached.
 *
 * Arguments:
 * str- pointer to array of characters to tokenize
 * sepchr- array of characters to use as delimiters
 *
 * Returns:
 * List of doubles parsed from string or NULL if error occurs.
 * User is responsible for freeing memory associated with returned List.
 */
List * GetDblListFromLineFStreamIO(const char * str, const char * sepchr)		{
	List * dbl_list			= NULL;				/* returned list of numbers parsed from str */
	ListElmt * lel			= NULL;				/* single element in list */	
	char * mlc_str			= NULL;				/* copy of string to be parsed */
	double * dbl_data		= NULL;				/* individual double token inserted into List */
	char * token 			= NULL;				/* ptr used to parse out tokens */	

	/* check args */
	if ( str == NULL || sepchr == NULL)	{
		ERR_ERROR_CONTINUE("Empty string supplied to string parsing routine. \n", ERR_ENOMEM);
		return dbl_list;
		}		
	if ( (dbl_list = InitListEmpty(free)) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for List. \n", ERR_ENOMEM);
		return dbl_list;
		}	
				
	if ( (mlc_str = (char *) malloc(sizeof(char) * (strlen(str) + 1))) == NULL )	{
		FreeList(dbl_list);
		ERR_ERROR_CONTINUE("Unable to copy string to be parsed into memory. \n", ERR_ENOMEM);
		return dbl_list;
		}
	strcpy(mlc_str, str);
	token = strtok(mlc_str, sepchr);
	do	{
		if ( token == NULL )	{
			break;
			}
		if ((dbl_data = (double *) malloc(sizeof(double))) != NULL)	{
			*dbl_data = atof(token);
			lel = LIST_TAIL(dbl_list);			
			if ( ListInsertNext(dbl_list, lel, dbl_data) )	{
				ERR_ERROR_CONTINUE("Problem inserting double into list. \n", ERR_EBADFUNC);
				}
			}
		token = strtok(NULL, sepchr);
	} while(1);	
	free(mlc_str);
	
	return dbl_list;	
	}
 
/*
 * Visibility:
 * global
 *
 * Description:
 * Takes a C-style string ('\0' terminated array of characters) and tokenizes the
 * string using any of the characters in sepchr as delimeters.
 * Each of the tokenized substrings are converted to type int and inserted
 * inside of a dynamically sized list.
 * The function terminates when the end of the string has been reached.
 *
 * Arguments:
 * str- pointer to array of characters to tokenize
 * sepchr- array of characters to use as delimiters
 *
 * Returns:
 * List of ints parsed from string or NULL if error occurs.
 * User is responsible for freeing memory associated with returned List.    
 */
List * GetIntListFromLineFStreamIO(const char * str, const char * sepchr)		{
	List * int_list			= NULL;				/* returned list of ints parsed from str */
	ListElmt * lel			= NULL;				/* single element in list */	
	char * mlc_str			= NULL;				/* copy of string to be parsed */
	int * int_data			= NULL;				/* individual int token inserted into List */
	char * token 			= NULL;				/* ptr used to parse out tokens */

	/* check args */
	if ( str == NULL || sepchr == NULL)	{
		ERR_ERROR_CONTINUE("Empty string supplied to string parsing routine. \n", ERR_ENOMEM);
		return int_list;
		}		
	if ( (int_list = InitListEmpty(free)) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for List. \n", ERR_ENOMEM);
		return int_list;
		}	
				
	if ( (mlc_str = (char *) malloc(sizeof(char) * (strlen(str) + 1))) == NULL )	{
		FreeList(int_list);
		ERR_ERROR_CONTINUE("Unable to copy string to be parsed into memory. \n", ERR_ENOMEM);
		return int_list;
		}
	strcpy(mlc_str, str);
	token = strtok(mlc_str, sepchr);
	do	{
		if ( token == NULL )	{
			break;
			}
		if ((int_data = (int *) malloc(sizeof(int))) != NULL)	{
			*int_data = atoi(token);
			lel = LIST_TAIL(int_list);			
			if ( ListInsertNext(int_list, lel, int_data) )	{
				ERR_ERROR_CONTINUE("Problem inserting int into List. \n", ERR_EBADFUNC);
				}
			}
		token = strtok(NULL, sepchr);
	} while(1);	
	free(mlc_str);
	
	return int_list;	
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Takes a C-style string ('\0' terminated array of characters) and tokenizes the
 * string using any of the characters in sepchr as delimeters.
 * Each of the tokenized substrings are converted to a C-style string ('\0'
 * terminated array of characters) and inserted inside of a dynamically sized list.
 * The function terminates when the end of the string has been reached.
 *
 * Arguments:
 * str- pointer to array of characters to tokenize
 * sepchr- array of characters to use as delimiters
 *
 * Returns:
 * List of ints parsed from string or NULL if error occurs.
 * User is responsible for freeing memory associated with returned List.    
 */
List * GetStrListFromLineFStreamIO(const char * str, const char * sepchr)	{
	List * str_list			= NULL;				/* returned list of strings parsed from str */
	ListElmt * lel			= NULL;				/* single element in list */	
	char * mlc_str			= NULL;				/* copy of string to be parsed */
	char * str_data			= NULL;				/* individual string token inserted into List */
	char * token 			= NULL;				/* ptr used to parse out tokens */

	/* check args */
	if ( str == NULL || sepchr == NULL)	{
		ERR_ERROR_CONTINUE("Empty string supplied to string parsing routine. \n", ERR_ENOMEM);
		return str_list;
		}		
	if ( (str_list = InitListEmpty(free)) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for List. \n", ERR_ENOMEM);
		return str_list;
		}	
				
	if ( (mlc_str = (char *) malloc(sizeof(char) * (strlen(str) + 1))) == NULL )	{
		FreeList(str_list);
		ERR_ERROR_CONTINUE("Unable to copy string to be parsed into memory. \n", ERR_ENOMEM);
		return str_list;
		}
	strcpy(mlc_str, str);
	token = strtok(mlc_str, sepchr);
	do	{
		if ( token == NULL )	{
			break;
			}
		if ((str_data = (char *) malloc(sizeof(char) * (strlen(token) + 1))) != NULL)	{
			strcpy(str_data, token);
			lel = LIST_TAIL(str_list);			
			if ( ListInsertNext(str_list, lel, str_data) )	{
				ERR_ERROR_CONTINUE("Problem inserting string into List. \n", ERR_EBADFUNC);
				}
			}
		token = strtok(NULL, sepchr);
	} while(1);	
	free(mlc_str);
	
	return str_list;	
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Takes an open and readable file stream and parses lines from file not preceded 
 * by comments into a two dimensional array of values.  The table is returned to
 * the user.  The number of columns in the table is calculated based upon the number
 * of elements in the FIRST non comment line reached in the stream.  This method 
 * expects that subsequent non-comment lines have a number of elements less than or 
 * equal to the number of elements in the first line. Upon return from this function,
 * the fstream will point to the end of file. 
 *
 * Arguments:
 * fstream- FILE pointer to open and readable stream
 * sepchr- array of characters to use as delimiters  
 * cmtchr- array of characters representing valid comment characters
 *			if NULL then function retrieves all lines remaining in stream
 *
 * Returns:
 * IntTwoDArray of values parsed from stream or NULL if not initialized.
 * User is responsible for freeing memory associated with returned TwoDArray.
 */  
IntTwoDArray * GetIntTwoDArrayTableFStreamIO(FILE * fstream, const char * sepchr, const char * cmtchr)	{
	IntTwoDArray * tbl 		= NULL;						/* returned table of values */
	List * line_list		= NULL;						/* List of lines in stream */
	ListElmt * line_elmt 	= NULL;						/* single element in line List */	
	List * int_list			= NULL;						/* List of values in one line of stream */
	ListElmt * int_elmt		= NULL;						/* single element in List */
	int * int_data			= NULL;						/* one int in entire table */
	char * line				= NULL;						/* one line from List of Lines */
	int is_tbl_init = 0, i = 0, j = 0;

	/* check args */
	if ( fstream == NULL )	{
		ERR_ERROR_CONTINUE("Unable to read stream passed to GetIntTwoDArrayTableFStreamIO. \n", ERR_EIOFAIL);
		return tbl;
		}

	/* retrieve all non comment lines from stream */
	if ( (line_list = GetListNonCommentLinesFStreamIO(fstream,cmtchr)) == NULL )	{
		ERR_ERROR_CONTINUE("List of lines not retrieved in GetIntTwoDArrayTableFStreamIO. \n", ERR_ENOMEM);
		return tbl;
		}

	/* parse each line in List and insert values in table */
	line_elmt = LIST_HEAD(line_list);
	while ( line_elmt != NULL )	{
		line = LIST_GET_DATA(line_elmt);
		if ( (int_list = GetIntListFromLineFStreamIO(line, sepchr)) != NULL )	{
			if ( is_tbl_init == 0 )	{
				/* initialize table from num rows and num cols */
				if ( (tbl = InitIntTwoDArraySizeIniValue(LIST_SIZE(line_list), LIST_SIZE(int_list), 0)) == NULL )	{
					ERR_ERROR_CONTINUE("Memory not allocated GetIntTwoDArrayTableFStreamIO. \n", ERR_ENOMEM);
					return tbl;
					}
				is_tbl_init = 1;
				}
			/* insert data from list into table */
			j = 0;
			int_elmt = LIST_HEAD(int_list);
			while (int_elmt != NULL && j < INTTWODARRAY_SIZE_COL(tbl) )	{
				int_data = LIST_GET_DATA(int_elmt);
				INTTWODARRAY_SET_DATA(tbl, i, j, *int_data);
				int_elmt = LIST_GET_NEXT_ELMT(int_elmt);
				j++;
				}
			i++;
			FreeList(int_list);
			}
		line_elmt = LIST_GET_NEXT_ELMT(line_elmt);
		}
		
	/* free memory for lines */
	FreeList(line_list);

	return tbl;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Takes an open and readable file stream and parses lines from file not preceded 
 * by comments into a two dimensional array of values.  The table is returned to
 * the user.  The number of columns in the table is calculated based upon the number
 * of elements in the FIRST non comment line reached in the stream.  This method 
 * expects that subsequent non-comment lines have a number of elements less than or 
 * equal to the number of elements in the first line. Upon return from this function,
 * the fstream will point to the end of file. 
 *
 * Arguments:
 * fstream- FILE pointer to open and readable stream
 * sepchr- array of characters to use as delimiters  
 * cmtchr- array of characters representing valid comment characters
 *			if NULL then function retrieves all lines remaining in stream
 *
 * Returns:
 * DblTwoDArray of values parsed from stream or NULL if not initialized.
 * User is responsible for freeing memory associated with returned TwoDArray.  
 */
DblTwoDArray * GetDblTwoDArrayTableFStreamIO(FILE * fstream, const char * sepchr, const char * cmtchr)	{
	DblTwoDArray * tbl 		= NULL;						/* returned table of values */
	List * line_list		= NULL;						/* List of lines in stream */
	ListElmt * line_elmt 	= NULL;						/* single element in line List */
	List * dbl_list			= NULL;						/* List of values in one line of stream */
	ListElmt * dbl_elmt		= NULL;						/* single element in dbl List */
	double * dbl_data		= NULL;						/* one double in entire table */
	char * line				= NULL;						/* line parsed from stream */
	int is_tbl_init = 0, i = 0, j = 0;

	/* check args */
	if ( fstream == NULL )	{
		ERR_ERROR_CONTINUE("Unable to read stream passed to GetDblTwoDArrayTableFStreamIO. \n", ERR_EIOFAIL);
		return tbl;
		}

	/* retrieve all non comment lines from stream */
	if ( (line_list = GetListNonCommentLinesFStreamIO(fstream,cmtchr)) == NULL )	{
		ERR_ERROR_CONTINUE("List of lines not retrieved in GetDblTwoDArrayTableFStreamIO. \n", ERR_ENOMEM);
		return tbl;
		}
	
	/* parse each line in List and insert values in table */
	line_elmt = LIST_HEAD(line_list);
	while ( line_elmt != NULL )	{
		line = LIST_GET_DATA(line_elmt);
		if ( (dbl_list = GetDblListFromLineFStreamIO(line, sepchr)) != NULL )	{
			if ( is_tbl_init == 0 )	{
				/* initialize table from num rows and num cols */
				if ( (tbl = InitDblTwoDArraySizeIniValue(LIST_SIZE(line_list), LIST_SIZE(dbl_list), 0.0)) == NULL )	{
					ERR_ERROR_CONTINUE("Memory not allocated GetDblTwoDArrayTableFStreamIO. \n", ERR_ENOMEM);
					return tbl;
					}
				is_tbl_init = 1;
				}
			/* insert data from list into table */
			j = 0;
			dbl_elmt = LIST_HEAD(dbl_list);
			while (dbl_elmt != NULL && j < DBLTWODARRAY_SIZE_COL(tbl) )	{
				dbl_data = LIST_GET_DATA(dbl_elmt);
				DBLTWODARRAY_SET_DATA(tbl, i, j, *dbl_data);
				dbl_elmt = LIST_GET_NEXT_ELMT(dbl_elmt);
				j++;
				}
			i++;
			FreeList(dbl_list);
			}
		line_elmt = LIST_GET_NEXT_ELMT(line_elmt);
		}
		
	/* free memory for lines */
	FreeList(line_list);

	return tbl;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Takes an open and readable file stream and parses lines from file not preceded 
 * by comments into a two dimensional array of values.  The table is returned to
 * the user.  The number of columns in the table is calculated based upon the number
 * of elements in the FIRST non comment line reached in the stream.  This method 
 * expects that subsequent non-comment lines have a number of elements less than or 
 * equal to the number of elements in the first line. Upon return from this function,
 * the fstream will point to the end of file. 
 *
 * Arguments:
 * fstream- FILE pointer to open and readable stream
 * sepchr- array of characters to use as delimiters  
 * cmtchr- array of characters representing valid comment characters
 *			if NULL then function retrieves all lines remaining in stream
 *
 * Returns:
 * StrTwoDArray of values parsed from stream or NULL if not initialized.
 * User is responsible for freeing memory associated with returned TwoDArray.  
 */
StrTwoDArray * GetStrTwoDArrayTableFStreamIO(FILE * fstream, const char * sepchr, const char * cmtchr)	{
	StrTwoDArray * tbl 		= NULL;						/* returned table of values */
	List * line_list		= NULL;						/* List of lines in stream */
	ListElmt * line_elmt 	= NULL;						/* single element in line List */	
	List * str_list			= NULL;						/* List of values in one line of stream */
	ListElmt * str_elmt		= NULL;						/* single element in List */
	char * str_data			= NULL;						/* one string in entire table */
	char * line				= NULL;						/* line parsed from stream */
	char * ini_value		= "NULL";
	int is_tbl_init = 0, i = 0, j = 0;

	/* check args */
	if ( fstream == NULL )	{
		ERR_ERROR_CONTINUE("Unable to read stream passed to GetStrTwoDArrayTableFStreamIO. \n", ERR_EIOFAIL);
		return tbl;
		}

	/* retrieve all non comment lines from stream */
	if ( (line_list = GetListNonCommentLinesFStreamIO(fstream,cmtchr)) == NULL )	{
		ERR_ERROR_CONTINUE("List of lines not retrieved in GetStrTwoDArrayTableFStreamIO. \n", ERR_ENOMEM);
		return tbl;
		}

	/* parse each line in List and insert values in table */
	line_elmt = LIST_HEAD(line_list);
	while ( line_elmt != NULL )	{
		line = LIST_GET_DATA(line_elmt);
		if ( (str_list = GetStrListFromLineFStreamIO(line, sepchr)) != NULL )	{
			if ( is_tbl_init == 0 )	{
				/* initialize table from num rows and num cols */
				if ( (tbl = InitStrTwoDArraySizeIniValue(LIST_SIZE(line_list), LIST_SIZE(str_list), ini_value)) == NULL )	{
					ERR_ERROR_CONTINUE("Memory not allocated GetStrTwoDArrayTableFStreamIO. \n", ERR_ENOMEM);
					return tbl;
					}
				is_tbl_init = 1;
				}
			/* insert data from list into table */
			j = 0;
			str_elmt = LIST_HEAD(str_list);
			while (str_elmt != NULL && j < STRTWODARRAY_SIZE_COL(tbl) )	{
				str_data = LIST_GET_DATA(str_elmt);
				STRTWODARRAY_SET_DATA(tbl, i, j, str_data);
				str_elmt = LIST_GET_NEXT_ELMT(str_elmt);
				j++;
				}
			i++;
			FreeList(str_list);
			}
		line_elmt = LIST_GET_NEXT_ELMT(line_elmt);
		}
		
	/* free memory for lines */
	FreeList(line_list);

	return tbl;
	}
		
/* end of FStreamIO.c */
