#ifndef IntXY_H
#define IntXY_H 

#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/* value for PI */
#define INTXY_PI										(3.141592654)

/* boolean types returned by the IsXXX functions */
#define INTXY_TRUE										(1)
#define INTXY_FALSE										(0)

/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/* structure representing an instance of a IntXY coordinate */
typedef struct IntXY_ IntXY;

struct IntXY_	{
	long int x;											/* coordinate in x direction */
	long int y;											/* coordinate in y direction */
	};

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

/* degree and radian conversion functions */
#define INTXY_DEG_TO_RAD(deg)							(((deg) * 2.0 * INTXY_PI) / 360.0)
#define INTXY_RAD_TO_DEG(rad)							(((rad) * 360.0) / (2.0 * INTXY_PI))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

IntXY * InitIntXYEmpty();

IntXY * InitIntXY(long int x, long int y);

int IntXYIsEqual(IntXY * xy1, IntXY * xy2);

int ixyIsEqual(long int x1, long int y1, long int x2, long int y2);

int IntXYCalcDist(IntXY * xy1, IntXY * xy2, double * dist);

int ixyCalcDist(long int x1, long int y1, long int x2, long int y2, double * dist);

int IntXYCalcBearing(IntXY * xyfrom, IntXY * xyto, double * theta);

int ixyCalcBearing(long int xf, long int yf, long int xt, long int yt, double * theta);

void FreeIntXY(void * vptr);

#endif IntXY_H	/* end of IntXY.h */
