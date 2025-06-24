#ifndef DblXY_H
#define DblXY_H 

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
#define DBLXY_EPSILON									(1.0e-6)

/* value for PI */
#define DBLXY_PI										(3.141592654)

/* boolean types returned by the IsXXX functions */
#define DBLXY_TRUE										(1)
#define DBLXY_FALSE										(0)

/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/* structure representing an instance of a DblXY coordinate */
typedef struct DblXY_ DblXY;
			
struct DblXY_	{
	double x;											/* coordinate in x direction */
	double y;											/* coordinate in y direction */
	};

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

/* macro used for floating point test of equality */
#define DBLXY_FP_EQUAL(a,b)								(fabs((a) - (b)) < DBLXY_EPSILON)

/* macro used for floating point test of zero */
#define DBLXY_FP_IS_ZERO(a)								(fabs(a) < DBLXY_EPSILON)

/* macro used for floating point test less than zero */
#define DBLXY_FP_LT_ZERO(a)								((a) < (DBLXY_EPSILON))

/* macro used for floating point test greater than zero */
#define DBLXY_FP_GT_ZERO(a)								((a) > (DBLXY_EPSILON))	

/* degree and radian conversion functions */
#define DBLXY_DEG_TO_RAD(deg)							(((deg) * 2.0 * DBLXY_PI) / 360.0)
#define DBLXY_RAD_TO_DEG(rad)							(((rad) * 360.0) / (2.0 * DBLXY_PI))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

DblXY * InitDblXYEmpty();

DblXY * InitDblXY(double x, double y);

int DblXYIsEqual(DblXY * xy1, DblXY * xy2);

int dxyIsEqual(double x1, double y1, double x2, double y2);

int DblXYCalcDist(DblXY * xy1, DblXY * xy2, double * dist);

int dxyCalcDist(double x1, double y1, double x2, double y2, double * dist);

int DblXYCalcBearing(DblXY * xyfrom, DblXY * xyto, double * theta);

int dxyCalcBearing(double xf, double yf, double xt, double yt, double * theta);

void FreeDblXY(void * vptr);

#endif DblXY_H	/* end of DblXY.h */
