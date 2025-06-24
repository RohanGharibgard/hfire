#ifndef	RGBColors_H
#define RGBColors_H

#include <stdlib.h>

#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/* factor used to darken or lighten supplied rgb values */
#define RGBCOLORS_DARK_LIGHT_FACTOR							(0.7)

enum EnumBasicColor_	{
	EnumWhite		= 0,
	EnumLightGray	= 1,
	EnumGray		= 2,
	EnumDarkGray	= 3,
	EnumBlack		= 4,
	EnumRed			= 5,
	EnumPink		= 6,
	EnumOrange		= 7,
	EnumYellow		= 8,
	EnumGreen		= 9,
	EnumMagenta		= 10,
	EnumCyan		= 11,
	EnumBlue		= 12
	};
	 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

typedef enum EnumBasicColor_ EnumBasicColor;

typedef struct RGB_ RGB;

struct RGB_	{
	int r;
	int g;
	int b;
	};
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */

/* returns the smalller of the two arguments */
#define RGBCOLORS_MIN(x, y)             							(((x) < (y)) ? (x) : (y))

/* returns the larger of the two arguments */
#define RGBCOLORS_MAX(x, y)             							(((x) > (y)) ? (x) : (y))
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

RGB * InitRGB(int red, int green, int blue);

RGB * InitRGBBasicColor(EnumBasicColor c);

int RGBColorsGetBasicColorRed(EnumBasicColor c);

int RGBColorsGetBasicColorGreen(EnumBasicColor c);

int RGBColorsGetBasicColorBlue(EnumBasicColor c);

void RGBColorsGetDarkerColor(int red, int green, int blue, int * dred, int * dgreen, int * dblue);

void RGBColorsGetLighterColor(int red, int green, int blue, int * dred, int * dgreen, int * dblue);

void FreeRGB(void * vptr);

/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */
  
#endif RGBColors_H		/* end of RGBColors.h */
