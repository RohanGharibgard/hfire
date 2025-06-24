#include "IntXY.h"

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
 * Empty IntXY structure
 */
IntXY * InitIntXYEmpty()	{
	IntXY * xy = (IntXY *) malloc(sizeof(IntXY));
	if ( xy == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for IntXY. \n", ERR_ENOMEM);
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
 * x- int of x coordinate value
 * y- int of y coordindate value
 *
 * Returns:
 * IntXY structure with x and y
 */
IntXY * InitIntXY(long int x, long int y)	{
	IntXY * xy = InitIntXYEmpty();
	if ( xy == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for IntXY. \n", ERR_ENOMEM);
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
 * Tests xy pair for equality. Both x and y must be equal to return INTXY_TRUE.
 * Ensures arguments are not NULL prior to comparison.
 *
 * Arguments:
 * xy1- an IntXY struct with x and y values
 * xy2- an IntXY struct with x and y values
 *
 * Returns:
 * INTXY_TRUE if equal, INTXY_FALSE if false
 */
int IntXYIsEqual(IntXY * xy1, IntXY * xy2)	{
	if ( xy1 == NULL || xy2 == NULL )	{
		ERR_ERROR_RETURN("Unable to determine equality, one structure not initialized. \n", ERR_EFAULT, INTXY_FALSE);
		}
	return ( (xy1->x == xy2->x) && (xy1->y == xy2->y) );
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Tests xy pair for equality. Both x and y must be equal to return INTXY_TRUE.
 * No error checking of arguments is performed.
 *
 * Arguments:
 * x1- x coordinate value
 * y1- y coordinate value
 * x2- x coordinate value
 * y2- y coordinate value
 *
 * Returns:
 * INTXY_TRUE if equal, INTXY_FALSE if false
 */
int ixyIsEqual(long int x1, long int y1, long int x2, long int y2)	{
	return ( (x1 == x2) && (y1 == y2) );
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
 * xy1- an IntXY struct with x and y values
 * xy2- an IntXY struct with x and y values
 * dist- when function returns argument contains value of distance
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int IntXYCalcDist(IntXY * xy1, IntXY * xy2, double * dist)	{
	long int dx, dy;
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
 * No error checking of arguments is performed.
 *
 * Arguments:
 * x1- x coordinate value
 * y1- y coordinate value
 * x2- x coordinate value
 * y2- y coordinate value
 * dist- when function returns argument contains value of distance
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int ixyCalcDist(long int x1, long int y1, long int x2, long int y2, double * dist)	{
	long int dx = x1 - x2;
	long int dy = y1 - y2;
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
 * Ensures arguments are not NULL prior to calculation.
 *
 * Arguments:
 * xyfrom- an IntXY struct with x and y values
 * xyto- an IntXY struct with x and y values
 * theta- when function returns argument contains value of bearing in radians
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int IntXYCalcBearing(IntXY * xyfrom, IntXY * xyto, double * theta)	{
	if ( xyfrom == NULL || xyto == NULL )	{
		ERR_ERROR("Unable to calculate bearing, one structure not initialized. \n", ERR_EFAULT);
		}
	*theta = atan2(xyto->y - xyfrom->y, xyto->x - xyfrom->x);
	return ERR_SUCCESS;	
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Calculates bearing between two xy pairs. Bearing returned in arg theta.
 * The value of theta is in range -PI (-180) to PI (180).
 * No error checking of arguments is performed.
 *
 * Arguments:
 * xf- "from" x coordinate value
 * yf- "from" y coordinate value
 * xt- "to" x coordinate value
 * yt- "to" y coordinate value
 * theta- when function returns argument contains value of bearing in radians
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int ixyCalcBearing(long int xf, long int yf, long int xt, long int yt, double * theta)	{
	*theta = atan2(yt - yf, xt - xf);
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
 * vptr- an IntXY struct with x and y values
 *
 * Returns:
 * NONE
 */
void FreeIntXY(void * vptr)	{
	IntXY * xy = NULL;
	if ( vptr != NULL )	{
		xy = (IntXY *) vptr;
		free(xy);										/* free the IntXY structure */
		}
	xy = NULL;
	return;
	}

/* end of IntXY.c */
