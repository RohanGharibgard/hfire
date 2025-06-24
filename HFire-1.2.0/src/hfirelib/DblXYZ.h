#ifndef DblXYZ_H
#define DblXYZ_H 

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
#define DBLXYZ_EPSILON									(1.0e-6)

/* value for PI */
#define DBLXYZ_PI										(3.141592654)

/* boolean types returned by the IsXXX functions */
#define DBLXYZ_TRUE										(1)
#define DBLXYZ_FALSE									(0)

/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/* structure representing an instance of a DblXYZ coordinate */
typedef struct DblXYZ_ DblXYZ;

struct DblXYZ_	{
	double x;											/* coordinate in x direction */
	double y;											/* coordinate in y direction */
	double z;											/* coordinate in z direction */
	};

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

/* macro used for floating point test of equality */
#define DBLXYZ_FP_EQUAL(a,b)							(fabs((a) - (b)) < DBLXYZ_EPSILON)

/* macro used for floating point test of zero */
#define DBLXYZ_FP_IS_ZERO(a)							(fabs(a) < DBLXYZ_EPSILON)

/* macro used for floating point test less than zero */
#define DBLXYZ_FP_LT_ZERO(a)							((a) < (DBLXYZ_EPSILON))

/* macro used for floating point test greater than zero */
#define DBLXYZ_FP_GT_ZERO(a)							((a) > (DBLXYZ_EPSILON))	

/* degree and radian conversion functions */
#define DBLXYZ_DEG_TO_RAD(deg)							(((deg) * 2.0 * DBLXYZ_PI) / 360.0)
#define DBLXYZ_RAD_TO_DEG(rad)							(((rad) * 360.0) / (2.0 * DBLXYZ_PI))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

DblXYZ * InitDblXYZEmpty();

DblXYZ * InitDblXYZ(double x, double y, double z);

int DblXYZIsEqual(DblXYZ * xyz1, DblXYZ * xyz2);

int dxyzIsEqual(double x1, double y1, double z1, double x2, double  y2, double z2);

int DblXYZCalcDist(DblXYZ * xyz1, DblXYZ * xyz2, double * dist);

int dxyzCalcDist(double x1, double y1, double z1, double x2, double  y2, double z2, double * dist);

int DblXYZCalcBearing(DblXYZ * xyzfrom, DblXYZ * xyzto, double * theta, double * vert);

int dxyzCalcBearing(double xf, double yf, double zf, double xt, double yt, double zt,
														double * theta, double * vert);

void FreeDblXYZ(void * vptr);

#endif DblXYZ_H	/* end of DblXYZ.h */
