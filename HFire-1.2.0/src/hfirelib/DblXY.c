#include "DblXY.h"

/*
 * Visibility:
 * global
 *
 * Description:
 * Initializes structure with empty members
 *
 * Arguments:
 * NONE
 *
 * Returns:
 * Empty DblXY structure
 */
DblXY * InitDblXYEmpty()	{
	DblXY * xy = (DblXY *) malloc(sizeof(DblXY));
	if ( xy == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for DblXY. \n", ERR_ENOMEM);
		}
	return xy;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Initializes structure with members having values of arguments
 *
 * Arguments:
 * x- double of x coordinate value
 * y- double of y coordindate value
 *
 * Returns:
 * DblXY structure with x and y
 */
DblXY * InitDblXY(double x, double y)	{
	DblXY * xy = InitDblXYEmpty();
	if ( xy == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for DblXY. \n", ERR_ENOMEM);
		}
	else	{
		xy->x = x;
		xy->y = y;
		}
	return xy;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Tests xy pair for floating point equality. 
 * Both x and y must be equal to return DBLXY_TRUE.
 * Ensures arguments are not NULL prior to comparison. 
 *
 * Arguments:
 * xy1- a DblXY struct with x and y values
 * xy2- a DblXY struct with x and y values
 *
 * Returns:
 * DBLXY_TRUE if equal, DBLXY_FALSE if false
 */
int DblXYIsEqual(DblXY * xy1, DblXY * xy2)	{
	if ( xy1 == NULL || xy2 == NULL )	{
		ERR_ERROR_RETURN("Unable to determine equality, one structure not initialized. \n", ERR_EFAULT, DBLXY_FALSE);
		}
	return ( DBLXY_FP_EQUAL(xy1->x, xy2->x) && DBLXY_FP_EQUAL(xy1->y, xy2->y) );
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Tests xy pair for floating point equality. 
 * Both x and y must be equal to return DBLXY_TRUE.
 * No error checing of arguments is performed.
 *
 * Arguments:
 * x1- x coordinate
 * y1- y coordinate
 * x2- x coordinate
 * y2- y coordinate
 *
 * Returns:
 * DBLXY_TRUE if equal, DBLXY_FALSE if false
 */
int dxyIsEqual(double x1, double y1, double x2, double y2)	{
	return ( DBLXY_FP_EQUAL(x1, x2) && DBLXY_FP_EQUAL(y1, y2) );
	}
	
/*
 * Visibility:
 * global
 *
 * Description:
 * Calculates planar distance between two xy pairs. Distance returned in arg dist.
 * Ensures arguments are not NULL prior to calculation. 
 *
 * Arguments:
 * xy1- a DblXY struct with x and y values
 * xy2- a DblXY struct with x and y values
 * dist- when function returns argument contains value of distance
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int DblXYCalcDist(DblXY * xy1, DblXY * xy2, double * dist)	{
	double dx, dy;
	if ( xy1 == NULL || xy2 == NULL )	{
		ERR_ERROR("Unable to calculate distance, one structure not initialized. \n", ERR_EFAULT);
		}
	dx = xy1->x - xy2->x;
	dy = xy1->y - xy2->y;	
	*dist = sqrt( (dx * dx) + (dy * dy) );
	return ERR_SUCCESS;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Calculates planar distance between two xy pairs. Distance returned in arg dist.
 * No error checing of arguments is performed. 
 *
 * Arguments:
 * x1- x coordinate
 * y1- y coordinate
 * x2- x coordinate
 * y2- y coordinate
 * dist- when function returns argument contains value of distance
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int dxyCalcDist(double x1, double y1, double x2, double y2, double * dist)	{
	double dx = x1 - x2;
	double dy = y1 - y2;	
	*dist = sqrt( (dx * dx) + (dy * dy) );
	return ERR_SUCCESS;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Calculates bearing between two xy pairs. Bearing returned in arg theta.
 * The value of theta is in range -PI (-180) to PI (180).
 * Ensures arguments are not NULL prior to comparison. 
 *
 * Arguments:
 * xyfrom- a DblXY struct with x and y values
 * xyto- a DblXY struct with x and y values
 * theta- when function returns argument contains value of bearing in radians
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	
int DblXYCalcBearing(DblXY * xyfrom, DblXY * xyto, double * theta)	{
	if ( xyfrom == NULL || xyto == NULL )	{
		ERR_ERROR("Unable to calculate bearing, one structure not initialized. \n", ERR_EFAULT);
		}
	*theta = atan2((xyto->y - xyfrom->y), (xyto->x - xyfrom->x));
	return ERR_SUCCESS;	
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Calculates bearing between two xy pairs. Bearing returned in arg theta.
 * The value of theta is in range -PI (-180) to PI (180).
 * No error checing is performed. 
 *
 * Arguments:
 * xf- "from" x coordinate
 * yf- "from" y coordinate
 * xt- "to" x coordinate
 * yt- "to" y coordinate
 * theta- when function returns argument contains value of bearing in radians
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	
int dxyCalcBearing(double xf, double yf, double xt, double yt, double * theta)	{
	*theta = atan2((yt - yf), (xt - xf));
	return ERR_SUCCESS;		
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Frees memory associated with xy structure.
 *
 * Arguments:
 * vptr- a DblXY struct with x and y values
 *
 * Returns:
 * NONE
 */	
void FreeDblXY(void * vptr)	{
	DblXY * dxy = NULL;
	if ( vptr != NULL )	{
		dxy = (DblXY *) vptr;
		free(dxy);										/* free the DblXY structure */
		}
	dxy = NULL;
	return;
	}
		
/* end of DblXY.c */
