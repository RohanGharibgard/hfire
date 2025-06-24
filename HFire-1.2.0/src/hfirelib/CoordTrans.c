#include "CoordTrans.h"

/*
 * Visibility:
 * global
 *
 * Description:
 * Takes raster row and column indecies and transforms to real world coordinates.
 * Performs a six parameter affine transformation of the form:
 * rwx =	Ax	+	By	+ 	C
 * rwy =	Dx	+ 	Ey	+	F
 *
 * where
 *
 * rwx = [(x_cellsz) * 	(j)] 	+ 	[(brot) 		* 	(i)] 	+	(xulcntr)
 * rwy = [(drot)	 * 	(j)]	+	[-(y_cellsz) 	*	(i)]	+ 	(yulcntr)
 *
 * Arguments:
 * i- row in raster to be converted to real world units
 * j- column in raster to be converted to real world units
 * x_cellsz- size of single raster cell in the x-dimension
 * y_cellsz- size of a single raster cell in the y-dimension
 * xulcntr- x coordinate of the center of the upper left cell in raster
 * yulcntr- y coordinate of the center of the upper left cell in raster
 * brot- B rotation term
 * drot- D rotation term
 * rwx- returned real world x coordinate
 * rwy- returned real world y coordinate
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened  
 */
int CoordTransSixParamRasterToRealWorld(int i, int j, double x_cellsz, double y_cellsz, double xulcntr, 
									double yulcntr, double brot, double drot, double * rwx, double * rwy)	{
 	*rwx = (x_cellsz * j) + (brot * i) + xulcntr;
 	*rwy = (drot * j) + (-y_cellsz * i) + yulcntr;
 	return ERR_SUCCESS;
	} 

/*
 * Visibility:
 * global
 *
 * Description:
 * Takes x and y coordinates in real world units and transforms to raster row
 * column indecies (0-based).
 *
 * Arguments:
 * rwx- real world x coordinate to transform to raster column index
 * rwy- real world y coordinate to transform to raster row index
 * x_cellsz- size of single raster cell in the x-dimension
 * y_cellsz- size of a single raster cell in the y-dimension
 * xulcntr- x coordinate of center of upper left cell in raster
 * yulcntr- y coordinate of center of upper left cell in raster
 * i- row in raster corresponding to rwy
 * j- column in raster corresponding to rwx
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened  
 */
int CoordTransRealWorldToRaster(double rwx, double rwy, double x_cellsz, double y_cellsz, double xulcntr, 
									double yulcntr, int * i, int * j)		{
	*i = (yulcntr - rwy) / y_cellsz;
	*j = (rwx - xulcntr) / x_cellsz;
	return ERR_SUCCESS;
	} 

/* end of CoordTrans.c */
