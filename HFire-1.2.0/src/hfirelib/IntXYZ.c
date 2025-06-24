#include "IntXYZ.h"
		
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
 * Empty IntXYZ structure
 */
IntXYZ * InitIntXYZEmpty()	{
	IntXYZ * xyz = (IntXYZ *) malloc(sizeof(IntXYZ));
	if ( xyz == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for IntXYZ. \n", ERR_ENOMEM);
		}
	return xyz;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Initializes structure with members having values of arguments
 *
 * Arguments:
 * x- long int of x coordinate value
 * y- long int of y coordindate value
 * z- long int of z coordinate value
 *
 * Returns:
 * IntXYZ structure with x and y and z
 */	
IntXYZ * InitIntXYZ(long int x, long int y, long int z)	{
	IntXYZ * xyz = InitIntXYZEmpty();
	if ( xyz == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for IntXYZ. \n", ERR_ENOMEM);
		}
	else	{
		xyz->x = x;
		xyz->y = y;
		xyz->z = z;
		}
	return xyz;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Tests xyz pair for equality. 
 * x and y and z must be equal to return INTXYZ_TRUE.
 * Ensures arguments are not NULL prior to comparison.
 *
 * Arguments:
 * xyz1- a IntXYZ struct with x and y and z values
 * xyz2- a IntXYZ struct with x and y and z values
 *
 * Returns:
 * INTXYZ_TRUE if equal, INTXYZ_FALSE if false
 */	
int IntXYZIsEqual(IntXYZ * xyz1, IntXYZ * xyz2)		{
	if ( xyz1 == NULL || xyz2 == NULL )	{
		ERR_ERROR_RETURN("Unable to determine equality, one structure not initialized. \n", ERR_EFAULT, INTXYZ_FALSE);
		}
	return ( (xyz1->x == xyz2->x ) && (xyz1->y == xyz2->y) && (xyz1->z == xyz2->z) );
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Tests xyz pair for equality. 
 * x and y and z must be equal to return INTXYZ_TRUE.
 * No error checking of arguments is performed.
 *
 * Arguments:
 * x1- x coordinate
 * y1- y coordinate
 * z1- z coordinate
 * x2- x coordinate
 * y2- y coordinate
 * z2- z coordinate 
 *
 * Returns:
 * INTXYZ_TRUE if equal, INTXYZ_FALSE if false
 */		
int ixyzIsEqual(long int x1, long int y1, long int z1, long int x2, long int y2, long int z2)	{
	return ( (x1 == x2) && (y1 == y2)  && (z1 == z2) );
	}	

/*
 * Visibility:
 * global
 *
 * Description:
 * Calculates planar distance between two xyz pairs. Distance returned in arg dist.
 * Ensures arguments are not NULL prior to comparison. 
 *
 * Arguments:
 * xyz1- a IntXYZ struct with x and y and z values
 * xyz2- a IntXYZ struct with x and y and z values
 * dist- when function returns argument contains value of distance
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int IntXYZCalcDist(IntXYZ * xyz1, IntXYZ * xyz2, double * dist)	{
	double dx, dy, dz;
	if ( xyz1 == NULL || xyz2 == NULL )	{
		ERR_ERROR("Unable to calculate distance, one structure not initialized. \n", ERR_EFAULT);
		}
	dx = xyz1->x - xyz2->x;
	dy = xyz1->y - xyz2->y;
	dz = xyz1->z - xyz2->z;	
	*dist = sqrt( (dx * dx) + (dy * dy) + (dz * dz) );
	return ERR_SUCCESS;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Calculates planar distance between two xyz pairs. Distance returned in arg dist.
 * No error checking of arguments is performed.
 *
 * Arguments:
 * x1- x coordinate
 * y1- y coordinate
 * z1- z coordinate
 * x2- x coordinate
 * y2- y coordinate
 * z2- z coordinate 
 * dist- when function returns argument contains value of distance
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int ixyzCalcDist(long int x1, long int y1, long int z1, long int x2, long int y2, long int z2, double * dist)	{
	long int dx = x1 - x2;
	long int dy = y1 - y2;
	long int dz = z1 - z2;
	*dist = sqrt( (dx * dx) + (dy * dy) + (dz * dz) );
	return ERR_SUCCESS;
	}
	
/*
 * Visibility:
 * global
 *
 * Description:
 * Calculates 2 bearings between two xyz pairs. 
 * Horizontal bearing returned in arg theta.
 * Vertical bearing returned in arg vert.
 * The value of theta is in range -PI (-180) to PI (180).
 * The value of vert is in the range -PI/2(-90) to PI/2(90). 
 * Ensures arguments are not NULL prior to comparison.  
 *
 * Arguments:
 * xyzfrom- a IntXYZ struct with x and y and z values
 * xyzto- a IntXYZ struct with x and y and z values
 * theta- when function returns argument contains value of horizontal bearing in radians
 * vert- when function returns argument contains value of vertical bearing in radians 
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */		
int IntXYZCalcBearing(IntXYZ * xyzfrom, IntXYZ * xyzto, double * theta, double * vert)	{
	double dx, dy, dz;
	if ( xyzfrom == NULL || xyzto == NULL )	{
		ERR_ERROR("Unable to calculate bearing, one structure not initialized. \n", ERR_EFAULT);
		}
	dx = xyzto->x - xyzfrom->x;
	dy = xyzto->y - xyzfrom->y;
	dz = xyzto->z - xyzfrom->z;
	/* get bearing */
	*theta = atan2(dy, dx);
	/* get vertical */
	if ( INTXYZ_FP_IS_ZERO(dz) )
		*vert = 0.0;
	else
		*vert = atan( dz / (sqrt( (dx * dx) + (dy * dy) )) );

	return ERR_SUCCESS;	
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Calculates 2 bearings between two xyz pairs. 
 * Horizontal bearing returned in arg theta.
 * Vertical bearing returned in arg vert.
 * The value of theta is in range -PI (-180) to PI (180).
 * The value of vert is in the range -PI/2(-90) to PI/2(90).
 * No error checking is performed. 
 *
 * Arguments:
 * xf- "from" x coordinate value
 * yf- "from" y coordinate value
 * xt- "to" x coordinate value
 * yt- "to" y coordinate value
 * theta- when function returns argument contains value of horizontal bearing in radians
 * vert- when function returns argument contains value of vertical bearing in radians 
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 
int ixyzCalcBearing(long int xf, long int yf, long int zf, long int xt, long int yt, long int zt,
 														double * theta, double * vert)	{
 	long int dx = xt - xf;
	long int dy = yt - yf;
	long int dz = zt - zf;
	/* get bearing */
	*theta = atan2(dy, dx);
	/* get vertical */
	if ( dz == 0 )
		*vert = 0.0;
	else
		*vert = atan( dz / (sqrt( (dx * dx) + (dy * dy) )) );

	return ERR_SUCCESS;	
 	}
 														
/*
 * Visibility:
 * global
 *
 * Description:
 * Frees memory associated with xyz structure.
 *
 * Arguments:
 * vptr- a IntXYZ struct with x and y and z values
 *
 * Returns:
 * NONE
 */		
void FreeIntXYZ(void * vptr)	{
	IntXYZ * dxyz = NULL;
	if ( vptr != NULL )	{
		dxyz = (IntXYZ *) vptr;
		free(dxyz);										/* free the IntXYZ structure */
		}
	dxyz = NULL;
	return;
	}
	
/* end of IntXYZ.c */
