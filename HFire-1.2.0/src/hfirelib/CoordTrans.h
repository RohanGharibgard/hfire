#ifndef	CoordTrans_H
#define CoordTrans_H

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
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */

/* converts the real world y lower left corner to the real world center y of the upper left cell in the raster */
#define COORD_TRANS_YLLCORNER_TO_YULCNTR(yll, cellsz, rows)					((yll) + ((rows)*(cellsz)) - ((cellsz) / 2))

/* converts the real world x lower left corner to the real world center x of the upper left cell in the raster */
#define COORD_TRANS_XLLCORNER_TO_XULCNTR(xll, cellsz)						((xll) + ((cellsz) / 2))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

int CoordTransSixParamRasterToRealWorld(int i, int j, double x_cellsz, double y_cellsz, double xulcntr, 
									double yulcntr, double brot, double drot, double * rwx, double * rwy);
 													
int CoordTransRealWorldToRaster(double rwx, double rwy, double x_cellsz, double y_cellsz, double xulcntr, 
									double yulcntr, int * i, int * j); 													
											
/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */
  
#endif CoordTrans_H		/* end of CoordTrans.h */
