#ifndef	FStreamIO_H
#define FStreamIO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "IntTwoDArray.h"
#include "DblTwoDArray.h"
#include "StrTwoDArray.h"
#include "List.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/* initial size of growable character buffer used by GetLineFStreamIO */
#define FSTREAMIO_INITIAL_SCAN_LINE_SIZE						(125)
 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */
 
char * GetLineFStreamIO(FILE * fstream);

List * GetListNonCommentLinesFStreamIO(FILE * fstream, const char * cmtchr);

char * GetValFromKeyStringFStreamIO(FILE * fstream, const char * keystr, const char * sepchr, const char * cmtchr);

int IsKeyStringPresentFStreamIO(FILE * fstream, const char * keystr, const char * cmtchr);

List * GetDblListFromLineFStreamIO(const char * str, const char * sepchr);

List * GetIntListFromLineFStreamIO(const char * str, const char * sepchr);

List * GetStrListFromLineFStreamIO(const char * str, const char * sepchr);

IntTwoDArray * GetIntTwoDArrayTableFStreamIO(FILE * fstream, const char * sepchr, const char * cmtchr);

DblTwoDArray * GetDblTwoDArrayTableFStreamIO(FILE * fstream, const char * sepchr, const char * cmtchr);

StrTwoDArray * GetStrTwoDArrayTableFStreamIO(FILE * fstream, const char * sepchr, const char * cmtchr);

#endif FStreamIO_H		/* end of FStreamIO.h */
