#ifndef GridData_H
#define GridData_H
		
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ByteTwoDArray.h"
#include "IntTwoDArray.h"
#include "LIntTwoDArray.h"
#include "DblTwoDArray.h"
#include "FltTwoDArray.h"
#include "FStreamIO.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/* define the filename extension separator */
#define GRIDDATA_FILE_EXTENSION_SEPARATOR		(".")

/* filename extension for ascii main files */
#define GRIDDATA_ASCII_GRID_EXTENSION 			(".asc")

/* filename extension for binary main files */
#define GRIDDATA_BINARY_GRID_EXTENSION 			(".flt")

/* filename extension for binary header files */
#define GRIDDATA_BINARY_GRID_HEADER_EXTENSION 	(".hdr")

/* characters used to separate tokens in header file */
#define GRIDDATA_HEADER_SEP_CHARS				(" ")
		
/* keywords used in grid header */
#define GRIDDATA_KEYWORD_NCOLS 					("ncols")
#define GRIDDATA_KEYWORD_NROWS					("nrows")
#define GRIDDATA_KEYWORD_XLLCORNER				("xllcorner")
#define GRIDDATA_KEYWORD_YLLCORNER 				("yllcorner")
#define GRIDDATA_KEYWORD_CELLSIZE				("cellsize")
#define GRIDDATA_KEYWORD_NODATA_value			("NODATA_value")
#define GRIDDATA_KEYWORD_BYTEORDER				("byteorder")
#define GRIDDATA_KEYWORD_BYTEORDER_MSB			("MSBFIRST")
#define GRIDDATA_KEYWORD_BYTEORDER_LSB			("LSBFIRST")

/* enum used to identify the different possible grid types */
enum EnumGridType_ { 
	EnumBitGrid 	= 1,
	EnumByteGrid 	= 2,
	EnumIntGrid 	= 3,
	EnumLIntGrid	= 4,
	EnumFltGrid		= 5,
	EnumDblGrid 	= 6
	};
	
/* enum used to identify the different possible grid formats */
enum EnumGridFormat_ {
	EnumAsciiGrid	= 1,
	EnumBinaryGrid	= 2
	};

/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */
 
/* enum used to distinguish grid types */
typedef enum EnumGridType_ EnumGridType;

/* enum used to distinguish grid formats */
typedef enum EnumGridFormat_ EnumGridFormat;

/* structure used to store grid data */
typedef struct GridData_ GridData;

/* structure used to store 2d array */
typedef union GridArray_ GridArray;

/* structure used to store grid header information */
typedef struct GridHeaderInfo_ GridHeaderInfo;

/* structure used to store grid file information */
typedef struct GridFileInfo_ GridFileInfo;

/* structure used to store grid data */
struct GridData_	{
	EnumGridType 	gtype;			/* enumeration of possible grid data types */
	GridArray 		* arr;			/* underlying two dimensional array of data */
	GridHeaderInfo 	* ghdr;			/* structure storing header information */
	GridFileInfo 	* gfinfo;		/* (optional) structure storing file information */
	};

union GridArray_	{
	ByteTwoDArray 	* ba;			/* has data if gtype == EnumByteGrid */
	IntTwoDArray 	* ia;			/* has data if gtype == EnumIntGrid */
	LIntTwoDArray 	* lia;			/* has data if gtype == EnumLIntGrid */
	FltTwoDArray 	* fa;			/* has data if gtype == EnumFltGrid */		
	DblTwoDArray 	* da;			/* has data if gtype == EnumDblGrid */
	};
		
struct GridHeaderInfo_	{
	int nrows;						/* number of rows in the dataset */
	int ncols;						/* number of columns in the dataset */
	double xllcorner;				/* x cordinate of lower left corner */
	double yllcorner;				/* y coordinate of lower left corner */
	int cellsize;					/* cellsize in map units */
	int NODATA_value;				/* (optional) value of cells containing NODATA */
	int is_msbfirst;				/* (optional) 1 if BYTEORDER is 'MSBFIRST', else 0 */
	};
	
struct GridFileInfo_		{
	char * main_file;				/* main grid filename- for ascii both header and data in same file */
	char * header_file;				/* (optional) relevant when format is binary, name of .hdr filename */
	EnumGridFormat gfmt;			/* enumeration of possible grid formats */
	long int main_file_sz;			/* file size of main data file- for ascii files this includes header */
	long int bytes_read;			/* number of bytes read so far from main_file */
	};

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

/* implements a polymorphic type dispatch system using GridData */
#define GRID_DATA_GET_DATA(gd, row, col, data)		do	{							\
			switch((gd)->gtype)	{													\
				case EnumByteGrid:													\
					(data) = BYTETWODARRAY_GET_DATA((gd)->arr->ba, (row), (col));	\
					break;															\
				case EnumIntGrid:													\
					(data) = INTTWODARRAY_GET_DATA((gd)->arr->ia, (row), (col));	\
					break;															\
				case EnumLIntGrid:													\
					(data) = LINTTWODARRAY_GET_DATA((gd)->arr->lia, (row), (col));	\
					break;															\
				case EnumFltGrid:													\
					(data) = FLTTWODARRAY_GET_DATA((gd)->arr->fa, (row), (col));	\
					break;															\
				case EnumDblGrid:													\
					(data) = DBLTWODARRAY_GET_DATA((gd)->arr->da, (row), (col));	\
					break;															\
				}																	\
		}	while(0)

/* implements a polymorphic type dispatch system using GridData */
#define GRID_DATA_SET_DATA(gd, row, col, data)		do	{										\
			switch((gd)->gtype)	{																\
				case EnumByteGrid:																\
					BYTETWODARRAY_SET_DATA((gd)->arr->ba, (row), (col), (unsigned char) (data));\
					break;																		\
				case EnumIntGrid:																\
					INTTWODARRAY_SET_DATA((gd)->arr->ia, (row), (col), (int) (data));			\
					break;																		\
				case EnumLIntGrid:																\
					LINTTWODARRAY_SET_DATA((gd)->arr->lia, (row), (col), (long int) (data));	\
					break;																		\
				case EnumFltGrid:																\
					FLTTWODARRAY_SET_DATA((gd)->arr->fa, (row), (col), (float) (data));			\
					break;																		\
				case EnumDblGrid:																\
					DBLTWODARRAY_SET_DATA((gd)->arr->da, (row), (col), (double) (data));		\
					break;																		\
				}																				\
		}	while(0)

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

/* INITIALIZATION */
GridData * InitGridDataFromBinaryRaster			(char * main_fname, char * header_fname, EnumGridType grid_type);
GridData * InitGridDataFromAsciiRaster			(char * main_fname, EnumGridType grid_type);

/* DATA EXPORT */
int ExportGridDataAsBinaryRaster				(GridData * gd, char * main_fname);
int ExportGridDataAsAsciiRaster					(GridData * gd, char * main_fname);

int ExportDblTwoDArrayAsBinaryRaster			(DblTwoDArray * arr, double xll, double yll, int cellsz, int nodata,
 													int ismsb, char * fname);
int ExportDblTwoDArrayAsAsciiRaster				(DblTwoDArray * arr, double xll, double yll, int cellsz, int nodata,
 													char * fname);
int ExportFltTwoDArrayAsBinaryRaster			(FltTwoDArray * arr, double xll, double yll, int cellsz, int nodata, 
													int ismsb, char * fname);
int ExportFltTwoDArrayAsAsciiRaster				(FltTwoDArray * arr, double xll, double yll, int cellsz, int nodata,
 													char * fname);
int ExportLIntTwoDArrayAsBinaryRaster			(LIntTwoDArray * arr, double xll, double yll, int cellsz, int nodata, 
													int ismsb, char * fname);
int ExportLIntTwoDArrayAsAsciiRaster			(LIntTwoDArray * arr, double xll, double yll, int cellsz, int nodata,
 													char * fname);
int ExportIntTwoDArrayAsBinaryRaster			(IntTwoDArray * arr, double xll, double yll, int cellsz, int nodata, 
													int ismsb, char * fname);
int ExportIntTwoDArrayAsAsciiRaster				(IntTwoDArray * arr, double xll, double yll, int cellsz, int nodata,
 													char * fname);
int ExportByteTwoDArrayAsBinaryRaster			(ByteTwoDArray * arr, double xll, double yll, int cellsz, int nodata, 
													int ismsb, char * fname);
int ExportByteTwoDArrayAsAsciiRaster			(ByteTwoDArray * arr, double xll, double yll, int cellsz, int nodata,
 													char * fname);

/* MEMORY MGT */
void FreeGridData								(void * vptr);

/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */
 
/* INITIALIZATION */
GridData * InitGridDataFromDblTwoDArray			(DblTwoDArray * arr, double xll, double yll, int cellsz, int nodata);
GridData * InitGridDataFromFltTwoDArray			(FltTwoDArray * arr, double xll, double yll, int cellsz, int nodata);
GridData * InitGridDataFromLIntTwoDArray		(LIntTwoDArray * arr,double xll, double yll, int cellsz, int nodata);
GridData * InitGridDataFromIntTwoDArray			(IntTwoDArray * arr, double xll, double yll, int cellsz, int nodata);
GridData * InitGridDataFromByteTwoDArray		(ByteTwoDArray * arr,double xll, double yll, int cellsz, int nodata);

GridHeaderInfo * InitGridHeaderInfoFromFile		(GridFileInfo * gfinfo);
GridHeaderInfo * InitGridHeaderInfoFromArgs 	(int nrows, int ncols,double xll, double yll, int cellsz, int nodata,
 													int ismsb);

/* DATA IMPORT */
DblTwoDArray * ImportBinaryRasterAsDblTwoDArray	(GridFileInfo * gfinfo, GridHeaderInfo * ghdr);
DblTwoDArray * ImportAsciiRasterAsDblTwoDArray	(GridFileInfo * gfinfo, GridHeaderInfo * ghdr);

FltTwoDArray * ImportBinaryRasterAsFltTwoDArray	(GridFileInfo * gfinfo, GridHeaderInfo * ghdr);
FltTwoDArray * ImportAsciiRasterAsFltTwoDArray	(GridFileInfo * gfinfo, GridHeaderInfo * ghdr);

LIntTwoDArray * ImportBinaryRasterAsLIntTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr);
LIntTwoDArray * ImportAsciiRasterAsLIntTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr);

IntTwoDArray * ImportBinaryRasterAsIntTwoDArray	(GridFileInfo * gfinfo, GridHeaderInfo * ghdr);
IntTwoDArray * ImportAsciiRasterAsIntTwoDArray	(GridFileInfo * gfinfo, GridHeaderInfo * ghdr);

ByteTwoDArray * ImportBinaryRasterAsByteTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr);
ByteTwoDArray * ImportAsciiRasterAsByteTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr);

/* MEMORY MGT */
void FreeGridArray								(void * vptr, EnumGridType grid_type);
void FreeGridHeaderInfo							(void * vptr);
void FreeGridFileInfo							(void * vptr);

#endif GridData_H	/* end of GridData.h */
