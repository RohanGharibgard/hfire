#include "DblXYZ.h"

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
 * Empty DblXYZ structure
 */
DblXYZ * InitDblXYZEmpty()	{
	DblXYZ * xyz = (DblXYZ *) malloc(sizeof(DblXYZ));
	if ( xyz == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for DblXYZ. \n", ERR_ENOMEM);
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
 * x- double of x coordinate value
 * y- double of y coordindate value
 * z- double of z coordinate value
 *
 * Returns:
 * DblXYZ structure with x and y and z
 */	
DblXYZ * InitDblXYZ(double x, double y, double z)	{
	DblXYZ * xyz = InitDblXYZEmpty();
	if ( xyz == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for DblXYZ. \n", ERR_ENOMEM);
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
 * Tests xyz pair for floating point equality. 
 * x and y and z must be equal to return DBLXYZ_TRUE.
 * Ensures arguments are not NULL prior to comparison. 
 *
 * Arguments:
 * xyz1- a DblXYZ struct with x and y and z values
 * xyz2- a DblXYZ struct with x and y and z values
 *
 * Returns:
 * DBLXYZ_TRUE if equal, DBLXYZ_FALSE if false
 */	
int DblXYZIsEqual(DblXYZ * xyz1, DblXYZ * xyz2)		{
	if ( xyz1 == NULL || xyz2 == NULL )	{
		ERR_ERROR_RETURN("Unable to determine equality, one structure not initialized. \n", ERR_EFAULT, DBLXYZ_FALSE);
		}
	return ( DBLXYZ_FP_EQUAL(xyz1->x, xyz2->x) && DBLXYZ_FP_EQUAL(xyz1->y, xyz2->y) 
				&& DBLXYZ_FP_EQUAL(xyz1->z, xyz2->z) );
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Tests xyz pair for floating point equality. 
 * x and y and z must be equal to return DBLXYZ_TRUE.
 * No error checking is performed.
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
 * DBLXYZ_TRUE if equal, DBLXYZ_FALSE if false
 */	 
int dxyzIsEqual(double x1, double y1, double z1, double x2, double  y2, double z2)	{
	return ( DBLXYZ_FP_EQUAL(x1, x2) && DBLXYZ_FP_EQUAL(y1, y2) && DBLXYZ_FP_EQUAL(z1, z2) );
	}
	
/*
 * Visibility:
 * global
 *
 * Description:
 * Calculates planar distance between two xyz pairs. Distance returned in arg dist.
 * Ensures arguments are not NULL prior to calculation. 
 *
 * Arguments:
 * xyz1- a DblXY struct with x and y and z values
 * xyz2- a DblXY struct with x and y and z values
 * dist- when function returns argument contains value of distance
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int DblXYZCalcDist(DblXYZ * xyz1, DblXYZ * xyz2, double * dist)	{
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
 * No error checking is performed.
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
int dxyzCalcDist(double x1, double y1, double z1, double x2, double  y2, double z2, double * dist)	{
	double dx = x1 - x2;
	double dy = y1 - y2;
	double dz = z1 - z2;	
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
 * Ensures arguments are not NULL prior to calculation.  
 *
 * Arguments:
 * xyzfrom- a DblXYZ struct with x and y and z values
 * xyzto- a DblXYZ struct with x and y and z values
 * theta- when function returns argument contains value of horizontal bearing in radians
 * vert- when function returns argument contains value of vertical bearing in radians 
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */		
int DblXYZCalcBearing(DblXYZ * xyzfrom, DblXYZ * xyzto, double * theta, double * vert)	{
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
	if ( DBLXYZ_FP_IS_ZERO(dz) )
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
 * xf- "from" x coordinate
 * yf- "from" y coordinate
 * zf- "from" z coordinate
 * xt- "to" x coordinate
 * yt- "to" y coordinate
 * zt- "to" z coordinate
 * theta- when function returns argument contains value of horizontal bearing in radians
 * vert- when function returns argument contains value of vertical bearing in radians 
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int dxyzCalcBearing(double xf, double yf, double zf, double xt, double yt, double zt,
														double * theta, double * vert)	{
	double dx = xt - xf;
	double dy = yt - yf;
	double dz = zt - zf;
	/* get bearing */
	*theta = atan2(dy, dx);
	/* get vertical */
	if ( DBLXYZ_FP_IS_ZERO(dz) )
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
 * vptr- a DblXYZ struct with x and y and z values
 *
 * Returns:
 * NONE
 */		
void FreeDblXYZ(void * vptr)	{
	DblXYZ * dxyz = NULL;
	if ( vptr != NULL )	{
		dxyz = (DblXYZ *) vptr;
		free(dxyz);										/* free the DblXYZ structure */
		}
	dxyz = NULL;
	return;
	}
	
/* end of DblXYZ.c */
