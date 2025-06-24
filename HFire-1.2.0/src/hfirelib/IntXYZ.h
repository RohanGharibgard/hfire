#ifndef IntXYZ_H
#define IntXYZ_H 

#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/* floating point value defining threshold for equality */
#define INTXYZ_EPSILON									(1.0e-6)

/* value for PI */
#define INTXYZ_PI										(3.141592654)

/* boolean types returned by the IsXXX functions */
#define INTXYZ_TRUE										(1)
#define INTXYZ_FALSE									(0)

/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/* structure representing an instance of a IntXYZ coordinate */
typedef struct IntXYZ_ IntXYZ;

struct IntXYZ_	{
	long int x;											/* coordinate in x direction */
	long int y;											/* coordinate in y direction */
	long int z;											/* coordinate in z direction */
	};

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

/* macro used for floating point test of zero */
#define INTXYZ_FP_IS_ZERO(a)							(fabs(a) < INTXYZ_EPSILON)

/* degree and radian conversion functions */
#define INTXYZ_DEG_TO_RAD(deg)							(((deg) * 2.0 * INTXYZ_PI) / 360.0)
#define INTXYZ_RAD_TO_DEG(rad)							(((rad) * 360.0) / (2.0 * INTXYZ_PI))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

IntXYZ * InitIntXYZEmpty();

IntXYZ * InitIntXYZ(long int x, long int y, long int z);

int IntXYZIsEqual(IntXYZ * xyz1, IntXYZ * xyz2);

int ixyzIsEqual(long int x1, long int y1, long int z1, long int x2, long int y2, long int z2);

int IntXYZCalcDist(IntXYZ * xyz1, IntXYZ * xyz2, double * dist);

int ixyzCalcDist(long int x1, long int y1, long int z1, long int x2, long int y2, long int z2, double * dist);

int IntXYZCalcBearing(IntXYZ * xyzfrom, IntXYZ * xyzto, double * theta, double * vert);

int ixyzCalcBearing(long int xf, long int yf, long int zf, long int xt, long int yt, long int zt,
 														double * theta, double * vert);

void FreeIntXYZ(void * vptr);

#endif IntXYZ_H	/* end of IntXYZ.h */
