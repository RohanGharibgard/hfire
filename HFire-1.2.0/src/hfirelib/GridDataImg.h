#ifndef	GridDataImg_H
#define GridDataImg_H

#include <stdlib.h>
#include <math.h>

#include "gd.h"
#include "gdfontt.h"
#include "gdfonts.h"
#include "gdfontmb.h"
#include "gdfontl.h"
#include "gdfontg.h"

#include "GridData.h"
#include "RGBColors.h"
#include "IntTwoDArray.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/* comment lines in icm files must begin with this character */
#define GRIDDATA_IMG_ICM_COMMENT_CHAR						("#")

/* non comment lines in icm file separate tokens using these characters */
#define GRIDDATA_IMG_ICM_SEP_CHAR							("=:")

/* comment lines in dcm files must begin with this character */
#define GRIDDATA_IMG_DCM_COMMENT_CHAR						("#")

/* non comment lines in dcm file separate tokens using these characters */
#define GRIDDATA_IMG_DCM_SEP_CHAR							("=:")

/* floating point value defining threshold for equality */
#define GRIDDATA_IMG_EPSILON								(1.0e-6)
	
enum EnumPosition_	{
	EnumCTRPostion		= 0,
	EnumULPosition		= 1,
	EnumLLPosition		= 2,
	EnumLRPosition		= 3,
	EnumURPosition		= 4
	};
		
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

typedef enum EnumPosition_ EnumPosition;

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

/* macro used for floating point test of equality */
#define GRIDDATA_IMG_FP_ARE_EQUAL(a,b)						(fabs((a) - (b)) < (GRIDDATA_IMG_EPSILON))
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

int GridDataImgExportIndexColorModelPng(GridData * gd, char * png_fname, int imgW, int imgH,
		char * title_text, gdFontPtr title_font, EnumPosition title_pos, int title_r, int title_g, int title_b,
		int nodata_r, int nodata_g, int nodata_b, int * red, int * green, int * blue, int icm_size);

int GridDataImgGetIndexColorModelFromFile(char * icm_file, int ** red, int ** green, int ** blue, int * icm_size);								

int GridDataImgExportDirectColorModelJpeg(GridData * gd, char * jpg_fname, int imgW, int imgH,
		char * title_text, gdFontPtr title_font, EnumPosition title_pos, int title_r, int title_g, int title_b,
		int nodata_r, int nodata_g, int nodata_b, 
		double * intervals, int * red, int * green, int * blue, int dcm_size, int quality);

int GridDataImgGetDirectColorModelFromFile(char * dcm_file, double ** intervals, 
		int ** red, int ** green, int ** blue, int * dcm_size);

/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */
  
#endif GridDataImg_H		/* end of GridDataImg.h */
