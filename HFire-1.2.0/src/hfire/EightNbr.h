/*! 
 * \file EightNbr.h
 * \brief Routines for iterating over neighborhoods with eight degrees of freedom.
 * 
 * \author Marco Morais
 * \date 2000-2001 
 *
 *	HFire (Highly Optmized Tolerance Fire Spread Model) Library
 *	Copyright (C) 2001 Marco Morais
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation; either
 *	version 2.1 of the License, or (at your option) any later version.
 *
 *	This library is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	Lesser General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public
 *	License along with this library; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#ifndef	EightNbr_H
#define EightNbr_H

#include <stdlib.h>

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/*!	\def EIGHTNBR_NUM_NBR_CELLS
 *	\brief identifies number of neigbors associated with a cell 
 */
#define EIGHTNBR_NUM_NBR_CELLS									(8)

/* enumerations used to represent nbr azimuths available with current NbrScheme */
enum  {
	EIGHTNBR_NORTH 			= 0,
	EIGHTNBR_NORTHEAST 		= 1,
	EIGHTNBR_EAST			= 2,
	EIGHTNBR_SOUTHEAST 		= 3,
	EIGHTNBR_SOUTH			= 4,
	EIGHTNBR_SOUTHWEST		= 5,
	EIGHTNBR_WEST			= 6,
	EIGHTNBR_NORTHWEST		= 7
	};
 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/*! used to quickly retrieve nbr col index: N, NE, E, SE, S, SW, W, NW */
static const int	egtnbr_col	[EIGHTNBR_NUM_NBR_CELLS] 	= {  0,    1,    1,     1,     0,    -1,    -1,    -1};

/*! used to quickly retrieve nbr row index: N, NE, E, SE, S, SW, W, NW */
static const int 	egtnbr_row	[EIGHTNBR_NUM_NBR_CELLS] 	= { -1,   -1,    0,     1,     1,     1,     0,    -1};

/*! used to quickly retrieve nbr azimuth (0-360): N, NE, E, SE, S, SW, W, NW */
static const double egtnbr_az	[EIGHTNBR_NUM_NBR_CELLS]	= {0.0, 45.0, 90.0, 135.0, 180.0, 225.0, 270.0, 315.0};

/*! used to quickly retrieve nbr cartesian distance: N, NE, E, SE, S, SW, W, NW */
static const double egtnbr_dist	[EIGHTNBR_NUM_NBR_CELLS]	= {1.0, 1.4142, 1.0, 1.4142, 1.0, 1.4142, 1.0, 1.4142};
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */

/*!	\def EIGHTNBR_ROW_INDEX_AT_AZIMUTH
 *	\brief macro used to retrieve nbr cell index at various azimuths 
 */
#define EIGHTNBR_ROW_INDEX_AT_AZIMUTH(i, azimuth)				((i) + egtnbr_row[azimuth])
 
/*!	\def EIGHTNBR_COL_INDEX_AT_AZIMUTH
 *	\brief  macro used to retrieve nbr cell index at various azimuths 
 */
#define EIGHTNBR_COL_INDEX_AT_AZIMUTH(j, azimuth)				((j) + egtnbr_col[azimuth])

/*!	\def EIGHTNBR_AZIMUTH_AS_DBL
 * 	\brief macro used to retrieve azimuth as double 
 */
#define EIGHTNBR_AZIMUTH_AS_DBL(azimuth)						(egtnbr_az[(azimuth)])

/*! \def EIGHTNBR_CART_DIST_AT_AZIMUTH
 *	\brief macro used to retrieve cartesian distance at azimuth 
 */
#define EIGHTNBR_CART_DIST_AT_AZIMUTH(azimuth, cellsz)			(((egtnbr_dist[(azimuth)]) * (cellsz)))
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */
 
#endif EightNbr_H		/* end of EightNbr.h */
