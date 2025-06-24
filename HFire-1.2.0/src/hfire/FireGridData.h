/*! 
 * \file FireGridData.h
 * \brief Routines for reading raster spatial data into simulation.
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
 
#ifndef	FireGridData_H
#define FireGridData_H

#include <stdlib.h>

#include "FireProp.h"
#include "GridData.h"
#include "ChHashTable.h"
#include "KeyVal.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* used in extracting keys related to Grid data */
#define FIRE_GRIDDATA_RASTER_FORMAT						("_RASTER_FORMAT")
#define FIRE_GRIDDATA_MAIN_FILE							("_RASTER_MAIN_FILE")
#define FIRE_GRIDDATA_HEADER_FILE						("_RASTER_HEADER_FILE")
#define FIRE_GRIDDATA_DATA_TYPE							("_RASTER_TYPE")
#define FIRE_GRIDDATA_ELEV_DATA							("ELEV")
#define FIRE_GRIDDATA_SLOPE_DATA						("SLOPE")
#define FIRE_GRIDDATA_ASPECT_DATA						("ASPECT")
#define FIRE_GRIDDATA_FUELS_PNV_DATA					("FUELS_PNV")
#define FIRE_GRIDDATA_FUELS_STATIC_DATA					("FUELS_STATIC")
#define FIRE_GRIDDATA_STD_AGE_DATA						("STAND_AGE")
#define FIRE_GRIDDATA_IGNITION_RSP_DATA					("IGNITION_RSP")

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

GridData * GetGridDataFromPropsFireGridData(ChHashTable * proptbl, char * data_keyword);

/*! \fn int IsGridDataMatchingFireGridData(GridData * agd, GridData * bgd)
 *	\brief Tests two GridData rasters to ensure header information identical for each.
 *	\sa GridData 
 *	\sa ChHashTable
 *	\param agd first GridData raster to compare
 *	\param bgd second GridData raster to compare
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */ 
int IsGridDataMatchingFireGridData(GridData * agd, GridData * bgd);
 
#endif FireGridData_H		/* end of FireGridData.h */
