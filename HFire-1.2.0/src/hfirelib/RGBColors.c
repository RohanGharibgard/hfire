#include "RGBColors.h"

/* indexes into underlying basic_colors array */
#define RGBCOLORS_RED_INDEX									(0)
#define RGBCOLORS_GREEN_INDEX								(1)
#define RGBCOLORS_BLUE_INDEX								(2)

static const int basic_colors [][3]  = { 
										{255,	255, 	255},	/* white */
										{192, 	192, 	192},	/* light gray */
										{128, 	128, 	128},	/* gray */
										{ 64,  	64, 	64},	/* dark gray */
										{  0,	0, 		0},		/* black */
										{255,	0,		0},		/* red */
										{255, 	175,	175},	/* pink */
										{255, 	200,	0},		/* orange */
										{255, 	255,	0},		/* yellow */
										{  0, 	255,	0},		/* green */
										{255,	0,		255},	/* magenta */
										{  0,	255,	255}, 	/* cyan */
										{  0,	0,		255}	/* blue */
										};

RGB * InitRGB(int red, int green, int blue)	{
	RGB * rgb = NULL;
	if ( (rgb = (RGB *) malloc(sizeof(RGB))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for RGB.", ERR_ENOMEM);
		}
	else	{
		rgb->r = red;
		rgb->g = green;
		rgb->b = blue;
		}
	return rgb;
	}

RGB * InitRGBBasicColor(EnumBasicColor c)	{
	return InitRGB(RGBColorsGetBasicColorRed(c), RGBColorsGetBasicColorGreen(c), RGBColorsGetBasicColorBlue(c));		
	}
	
int RGBColorsGetBasicColorRed(EnumBasicColor c)		{
	return basic_colors[c][RGBCOLORS_RED_INDEX];
	}

int RGBColorsGetBasicColorGreen(EnumBasicColor c)	{
	return basic_colors[c][RGBCOLORS_GREEN_INDEX];
	}
	
int RGBColorsGetBasicColorBlue(EnumBasicColor c)	{
	return basic_colors[c][RGBCOLORS_BLUE_INDEX];
	}
	
void RGBColorsGetDarkerColor(int red, int green, int blue, int * dred, int * dgreen, int * dblue)	{
	*dred = RGBCOLORS_MAX(((int)(red * RGBCOLORS_DARK_LIGHT_FACTOR)), 0);
	*dgreen = RGBCOLORS_MAX(((int)(green * RGBCOLORS_DARK_LIGHT_FACTOR)), 0);
	*dblue = RGBCOLORS_MAX(((int)(blue * RGBCOLORS_DARK_LIGHT_FACTOR)), 0);
	
	return;
	}

void RGBColorsGetLighterColor(int red, int green, int blue, int * dred, int * dgreen, int * dblue)	{
	int i = (int)(1.0 / (1.0 - RGBCOLORS_DARK_LIGHT_FACTOR));
	if ( red == 0 && green == 0 && blue == 0 )	{
		/* if it is black turn it gray */
		*dred = i;
		*dgreen = i;
		*dblue = i;
		return;
		}
		
	if ( red > 0 && red < i )		{
		red = i;
		}
	if ( green > 0 && green < i ) 	{
		green = i;
		}
	if ( blue > 0 && blue < i ) 	{
		blue = i;
		}
	*dred = RGBCOLORS_MIN(((int)(red / RGBCOLORS_DARK_LIGHT_FACTOR)), 255);
	*dgreen = RGBCOLORS_MIN(((int)(green / RGBCOLORS_DARK_LIGHT_FACTOR)), 255);
	*dblue = RGBCOLORS_MIN(((int)(blue / RGBCOLORS_DARK_LIGHT_FACTOR)), 255);
	
	return;
	}

void FreeRGB(void * vptr)	{
	RGB * rgb = NULL;
	if ( vptr != NULL )	{
		rgb = (RGB *) vptr;
		free(rgb);										/* free the RGB structure */
		}
	rgb = NULL;
	return;
	}
			 
/* end of RGBColors.c */
